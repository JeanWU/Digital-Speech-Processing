#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Vocab.h"
#include "VocabMap.h"
#include "File.h"
#include "Prob.h"
#include "Ngram.h"


using namespace std;

Vocab ZhuYin, Big5, voc;
VocabIndex graphidx[200][1000];
int Back_table[200][1000];

bool is_back(LogP prob, LogP back_prob,LogP look_up, LogP maxp){
    if ((prob+look_up)>maxp)  return true;
    return false;
}
int find_max_prob_idx(int idx){
  VocabIndex tmp_word = voc.getIndex(Big5.getWord(idx));
  if (tmp_word == Vocab_None)
        tmp_word = voc.getIndex(Vocab_Unknown);
  return tmp_word;
}

int find_max_prob(LogP max_p, int num_cad,LogP prob[]){
  int idx_col = -87;
    for(int i=0; i<num_cad;i++){
        if(prob[i]>max_p){
          max_p = prob[i];
          idx_col= i;
        }
    }
    return idx_col;
}

void print_ans(VocabString Word[], int count){
  for (int i = 0; i < count; i++)
    (i==count-1)? cout<<Word[i]<<endl : cout<<Word[i]<<" ";
}



int main (int argc, char* argv[]) {
    VocabMap map(ZhuYin, Big5);

    File textfile(argv[2], "r");
    File mapFile(argv[4], "r");
    map.read(mapFile);
    mapFile.close();


    Ngram lookup(voc, atoi(argv[8]));
    File lookupFile(argv[6], "r");
    lookup.read(lookupFile);
    lookupFile.close();




    char* buffer;
    while(buffer = textfile.getline()) {

        LogP prob[200][1000] = {{0.0}};
        int num_cad[200];

        VocabString sentence[maxWordsPerLine];

        Prob p;
        VocabIndex v_idx;
        VocabIndex empty_context[] = {Vocab_None};
        VocabIndex bi_context[] = {Vocab_None, Vocab_None};

        int count = Vocab::parseWords(buffer, &(sentence[1]), maxWordsPerLine);
        sentence[0] = "<s>";
        sentence[count+1] = "</s>";

        VocabMapIter iter(map, ZhuYin.getIndex(sentence[0]));
        iter.init();
      
        //  initialize
        int size = 0;
        while (iter.next(v_idx, p)) {
            VocabIndex cad_idx = find_max_prob_idx(v_idx);
            LogP logp = lookup.wordProb(cad_idx, empty_context);
            prob[0][size] = (logp == LogP_Zero)  ? -87 : logp;
            graphidx[0][size] = v_idx;
            Back_table[0][size] = -87;
            size++;
        }
        num_cad[0] = size;

        // viterbi
        for (int i = 1; i < count+2; i++) {
            VocabMapIter iter(map, ZhuYin.getIndex(sentence[i]));
            iter.init();
            size = 0;
            while (iter.next(v_idx, p)) {
                VocabIndex cad_idx = find_max_prob_idx(v_idx);

                // evaluate max prob
                LogP maxp = LogP_Zero;
                for (int j = 0; j < num_cad[i-1]; j++) {
                    bi_context[0] = find_max_prob_idx(graphidx[i-1][j]);
                    LogP logp = lookup.wordProb(cad_idx, bi_context);
                    LogP backoff = lookup.wordProb(cad_idx, empty_context);

                    // check back tracking
                    if (is_back(logp,backoff,prob[i-1][j],maxp)) {
                        maxp = logp+prob[i-1][j];
                        Back_table[i][size] = j;
                    }
                    else  logp == -87;
                    logp += prob[i-1][j];
                }
                prob[i][size] = maxp;
                graphidx[i][size] = v_idx;
                size++;
            }
            num_cad[i] = size;
        }

        int max_col = find_max_prob(LogP_Zero,num_cad[count+1],prob[count+1]);

        VocabString Word[maxWordLength];
        Word[0] = "<s>";
        Word[count+1] = "</s>";
        for (int i = count+1; i > 0; i--) {
            Word[i] = Big5.getWord(graphidx[i][max_col]);
            max_col = Back_table[i][max_col];
        }
        print_ans(Word,count+2);

    }
    textfile.close();

    return 0;
}

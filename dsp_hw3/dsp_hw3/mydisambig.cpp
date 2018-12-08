#include "File.h"
#include "Ngram.h"
#include "Prob.h"       // Probability
#include "LM.h"         // Language Model
#include "Vocab.h"      // Vocabular
#include "VocabMap.h"   // Vocabulary map

/* Reference
 * http://www.speech.sri.com/projects/srilm/manpages/
 */

#define FILE_LENGTH 20
using namespace std;

int find_index(int idx, Vocab voc, Vocab Big5);
LogP check_backoff(LogP logp, LogP backoff);
void Viterbi(char *input_text, char *input_map, char *input_lm, int order);
void find_max_path(int count, int bound, LogP prob[][1000], Vocab Big5, VocabIndex vidxpath[][1000], int backtrack[][1000], VocabString *finalpath);
void print_path(VocabString path[], int count);


int main(int argc, char *argv[]){
  int order = atoi(argv[8]);

  char input_text[FILE_LENGTH],   //1~10.txt
       input_map[FILE_LENGTH],   //ZhuYin-Big5.map
       input_lm[FILE_LENGTH];   //bigram.lm

  strcpy(input_text, argv[2]);
  strcpy(input_map, argv[4]);
  strcpy(input_lm, argv[6]);

  Viterbi(input_text, input_map, input_lm, order);

  return 0;
}


void Viterbi(char *input_text, char *input_map, char *input_lm, int order){
  Vocab voc, ZhuYin, Big5;

  //read input map and language model
  VocabMap map(ZhuYin, Big5);
  File mapfile(input_map, "r");
  map.read(mapfile);
  mapfile.close();

  Ngram lm(voc, order);
  File lmfile(input_lm, "r");
  lm.read(lmfile);
  lmfile.close();

  //read input text file
  File textfile(input_text, "r");
  char *buffer;
  //distinguish ambiguity line by line
  while(buffer=textfile.getline()){
    //define variables
    LogP prob[200][1000]={{0.0}};
    VocabIndex vidxpath[200][1000];
    int backtrack[200][1000];
    int candidate_cnt[200];

    Prob p;
    VocabIndex vidx;
    VocabIndex empty_context[]={Vocab_None};
    VocabIndex bigram_context[]={Vocab_None, Vocab_None};

    //parse line to words
    VocabString sentence[maxWordsPerLine];
    int count=Vocab::parseWords(buffer, &(sentence[1]), maxWordsPerLine);
    sentence[0]="<s>";
    sentence[count+1]="</s>";
    count+=2;

    //initialize
    VocabMapIter iter(map, ZhuYin.getIndex(sentence[0]));
    iter.init();
    int size=0;
    while (iter.next(vidx,p)){
      VocabIndex candidate_idx=find_index(vidx, voc, Big5);
      LogP logp=lm.wordProb(candidate_idx, empty_context);
      prob[0][size]=(logp == LogP_Zero)? -66: logp;
      vidxpath[0][size]=vidx;
      backtrack[0][size]=-1;
      ++size;
    }
    candidate_cnt[0]=size;

    //recursion
    for (int i=1; i<count; ++i){
      VocabMapIter iter(map, ZhuYin.getIndex(sentence[i]));
      iter.init();
      size=0;
      while (iter.next(vidx,p)){
        VocabIndex candidate_idx=find_index(vidx, voc, Big5);

        LogP maxp=LogP_Zero;
        for (int j=0; j<candidate_cnt[i-1]; ++j){
          bigram_context[0]=find_index(vidxpath[i-1][j], voc, Big5);

          LogP logp=lm.wordProb(candidate_idx, bigram_context);
          LogP backoff=lm.wordProb(candidate_idx, empty_context);
          logp=check_backoff(logp, backoff);
          logp+=prob[i-i][j];
          if (logp > maxp){
            maxp=logp;
            backtrack[i][size]=j;
          }
        }
        prob[i][size]=maxp;
        vidxpath[i][size]=vidx;
        ++size;
      }
      candidate_cnt[i]=size;
    }

    //find the path with maximum probability
    int bound=candidate_cnt[count-1];
    VocabString finalpath[maxWordLength];
    find_max_path(count, bound, prob, Big5, vidxpath, backtrack, finalpath);
    print_path(finalpath, count);
  }
  textfile.close();

}


void print_path(VocabString path[], int count){
  for(int i=0; i<count; ++i){
    if(i==count-1){
      cout<<path[i]<<endl;
    }
    else{
      cout<<path[i]<<" ";
    }
  }
}


void find_max_path(int count, int bound, LogP prob[][1000], Vocab Big5, VocabIndex vidxpath[][1000], int backtrack[][1000], VocabString *finalpath){
  LogP maxp=LogP_Zero;
  int max_col=-1;
  for (int j=0; j<bound; ++j){
    if (prob[count-1][j] > maxp){
      maxp=prob[count-1][j];
      max_col=j;
    }
  }

  //VocabString finalpath[maxWordLength];
  
  finalpath[0]="<s>";
  //print_path(final_path, count);

  finalpath[count-1]="</s>";
  for (int i=count-1; i>0; --i){
    finalpath[i]=Big5.getWord(vidxpath[i][max_col]);
    //cout<<finalpath[i]<<endl;
    max_col=backtrack[i][max_col];
  }
  
}


LogP check_backoff(LogP logp, LogP backoff){
  if (logp==backoff && logp==LogP_Zero){
    logp==-66;
  }
  return logp;
}


int find_index(int idx, Vocab voc, Vocab Big5){
  //check if the word can be found in voc,
  //if not, set return index == Vocab_Unknown
  VocabIndex re_idx=voc.getIndex(Big5.getWord(idx));
  if (re_idx==Vocab_None){
    re_idx=voc.getIndex(Vocab_Unknown);
  }
  return re_idx;
}


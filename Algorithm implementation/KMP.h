/**
  * @author Zizhao Wang
  * @date 2021/8/26
**/

#ifndef ALGORITHM_IMPLEMENTATION_KMP_H
#define ALGORITHM_IMPLEMENTATION_KMP_H
using namespace  std;

void PMKMP(string pattern, string source)
{
    size_t * next;
    size_t LengthPattern=strlen(pattern);
    size_t LengthSource=strlen(source);

    next=(int*)malloc((strlen(pattern)+10)* sizeof(int));
    if(!next)
    {
        printf("Memory allocation failure!");
        return ;
    }
    pattern="$"+pattern;
    source="$"+source;

    next[1]=0;
    for(size_t i=2,j=0;i<LengthPattern;i++)
    {
        while(j && pattern[i]!=pattern[j+1]) j=next[j];
        if(pattern[i]==pattern[j+1]) j++;
        next[i]=j
    }

    for(size_t i=1,j=0;i<LengthSource;i++)
    {
        while(j&&source[i]!=pattern[j+1]) j=next[j];
        if(pattern[j+1]==source[i]) j++;
        if(j==LengthPattern)
        {
            printf("Accurate location:%d %d \n",i-n,i);
            j=0;
        }
    }
    free(next);
}

#endif //ALGORITHM_IMPLEMENTATION_KMP_H

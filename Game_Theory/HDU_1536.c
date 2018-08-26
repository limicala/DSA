#include <stdio.h>
#include <string.h>
int main(){

    int k, S[10001];
    char r[10001];
    while(scanf("%d", &k) && k != 0){
        memset(r, 0, sizeof(r));
        for(int i = 0; i < k; i++){
            scanf("%d", &S[i]);
        }
        //bubbleSort(S, k);
        int SG[10001];
        int has[10001];
        memset(SG, 0, sizeof(SG));
        for(int i = 1; i <= 10000; i++){
            //memset(has, 0, sizeof(has));
            for(int j = 0; j < i; j++){
                has[j] = 0;
            }
            for(int j = 0; j < k; j++){
                int idx = i - S[j];
                if(idx >= 0){
                    has[SG[idx]] = 1;
                }
            }
            for(int j = 0; j < i; j++){
                if(has[j] == 0){
                    SG[i] = j;
                    break;
                }
            }
        }
        int m, n, count, result;
        scanf("%d", &m);
        for(int i = 0; i < m; i++){
            scanf("%d", &n);
            result = 0;
            while(n--){
                scanf("%d", &count);
                result ^= SG[count];
            }
            if(result == 0){
                r[i] = 'L';
            } else{
                r[i] = 'W';
            }
        }
        printf("%s\n", r);
    }
}
#include <stdio.h>
#include <string.h>
int main()
{
    int T, N, A;
    scanf("%d", &T);
    while(T--){
        int fullCount = 0, result = 0;
        scanf("%d", &N);
        int i;
        for(i = 0; i < N; i++){
            scanf("%d", &A);
            result ^= A;
            if(A > 1){
                fullCount += 1;
            }
        }
        // S0 || T2 lose
        if( (result != 0 && fullCount == 0) || (result == 0 && fullCount >= 2)){
            printf("Brother\n");
        }else {
            printf("John\n");
        }
    }
    return 0;
}

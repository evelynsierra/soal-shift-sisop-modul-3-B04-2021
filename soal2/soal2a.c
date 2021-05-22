#include <stdio.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define baris1 4
#define kolom1 3
#define baris2 3
#define kolom2 6

//const int baris1 = 4, baris2 = 3, kolom1 = 3, kolom2 = 6;
int mx1[baris1][kolom1], mx2[baris2][kolom2];
int i,j,k;

void matriksAwal(){
	printf("Input matriks 4x3\n");
	for (i = 0; i < baris1; i++) {
    	for (j = 0; j < kolom1; j++) {
      		scanf("%d", &mx1[i][j]);
   	 	}
  	}
  	printf("Input matriks 3x6\n");
	for (i = 0; i < baris2; i++) {
    	for (j = 0; j < kolom2; j++) {
      		scanf("%d", &mx2[i][j]);
   	 	}
  	}
}

int main() {
	matriksAwal();
	
	key_t key = 1234;
  	int (*value)[kolom2];

  	int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
  	value = shmat(shmid, NULL, 0);
  	
  	printf("Hasil Perkalian Matriks :\n");
  	for (i = 0; i < baris1; i++) {
    	for (j = 0; j < kolom2; j++) {
		value[i][j]=0;
    		for (k = 0; k< 3; k++){
      			value[i][j] += mx1[i][k] * mx2[k][j];
			}
      		printf("%d ",value[i][j]);
   	 	}
    	printf("\n");
  	}

  	shmdt(value);
  	
  	return 0;
}

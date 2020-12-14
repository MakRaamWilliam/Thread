#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define Max 500
int a[Max][Max];
int b[Max][Max];
int matrix[Max][Max];
int crow,ccol,mid;

//struct to enable pass them toghter in thread function
struct MatrixEle{
 int row;
 int col;
};

//thread func to get the ele mul
void * EleMul(void * eleval){
 struct MatrixEle *elem;
 elem =(struct  elem*) eleval;
 int row=elem->row;
 int col=elem->col;
 // we get row & col of the element so cal it by
 for(int i=0;i<mid;i++)
  matrix[row][col]+=a[row][i]*b[i][col];

 free(eleval);
 pthread_exit(NULL);
}

//thread func to get the row mul
void * RowMul(void * rowval){
 int row = (int) rowval;
 //first loop for each col in row
 //sec loop for each ele in col
 for(int i=0;i<ccol;i++){
   for(int j=0;j<mid;j++){
    matrix[row][i]+=a[row][j]*b[j][i];
   }
 }
 pthread_exit(NULL);
}

char  file3[50];
void ReadMats(char *argv[]){
 // read from file
 char  file1[50];
 char  file2[50];
 if(argv != NULL &&argv[0]!=NULL &&argv[1]!=NULL &&argv[2]!=NULL && argv[3]!= NULL){
   strcpy(file1,argv[1]);  strcat(file1,".txt");
   strcpy(file2,argv[2]);  strcat(file2,".txt");
   strcpy(file3,argv[3]);  strcat(file3,".out");
  // printf("%s %s %s\n",file1,file2,file3);
  }
  else{
   strcpy(file1,"a.txt");
   strcpy(file2,"b.txt");
   strcpy(file3,"c.out");
  }


 FILE *mat1 = fopen(file1,"r");
 FILE *mat2 = fopen(file2,"r");

 if(mat1==NULL || mat2==NULL ){
   printf("can not find the files\n");
   exit(-1);
   }
 //string to handle row= & col=
 char str1[50],str2[50];
 int rowa=0,cola=0;
 //read first matrix

 fscanf(mat1,"%s %s",&str1,&str2);
 int ind=4;
 while(ind<50 && str1[ind]!='\0'){
  rowa=rowa*10+str1[ind]-'0';ind++;
 }ind=4;
 while(ind<50 && str2[ind]!='\0'){
  cola=cola*10+str2[ind]-'0';
  ind++;
 }ind=4;
 //printf("a %d %d\n",rowa,cola);
 if(rowa > 500 || cola>500){
   printf("Dimension error\n");
   exit(0);
 }
 for(int i=0;i<rowa;i++){
  for(int j=0;j<cola;j++){
    fscanf(mat1,"%d",&a[i][j]);
   // printf("%d ",a[i][j]);
  }//printf("\n");
 }
 fclose(mat1);

 //read sec matrix
 int rowb=0,colb=0;
 fscanf(mat2,"%s %s",&str1,&str2);
 while(ind<50 && str1[ind]!='\0'){
  rowb=rowb*10+str1[ind]-'0';ind++;
 }ind=4;
 while(ind<50 && str2[ind]!='\0'){
  colb=colb*10+str2[ind]-'0';
  ind++;
 }ind=4;
 //check valide dim
 if(cola != rowb){
   printf("Dimension error\n");
   exit(0);
   }
//  printf("b %d %d\n",rowb,colb);
 for(int i=0;i<rowb;i++){
  for(int j=0;j<colb;j++){
    fscanf(mat2,"%d",&b[i][j]);
  //  printf("%d ",b[i][j]);
  }//printf("\n");
 }
 fclose(mat2);

 //assign dim of matrices
 mid=cola,crow=rowa,ccol=colb;

}


int main(int argc, char *argv[]){
 //read matrices

 ReadMats(argv);

 //create threads for each ele
 pthread_t threads2[crow * ccol];
 int conn=0;

 //start time
 struct timeval stop, start;
 gettimeofday(&start, NULL);

 for(int i=0;i<crow;i++){
  for(int j=0;j<ccol;j++){
  struct MatrixEle *tdata = malloc(sizeof(struct MatrixEle));
  tdata->row=i;
  tdata->col=j;
  //printf("can not create thread\n");
  if( pthread_create(&threads2[conn],NULL,EleMul,(void *)tdata) )
    printf("can not create thread\n");
  conn++;
  //free(tdata);
    }
 }

// join to wait all thread finish
for(int i=0;i<(crow*ccol);i++){
  pthread_join(threads2[i],NULL);
 }
 //stop time of threads
 gettimeofday(&stop, NULL);

 //open c file to write c
 FILE * matc = fopen(file3,"w");
fprintf(matc,"Thread Element\nrow= %d col= %d\n",crow,ccol);
//printf("Thread Element\nrow= %d col= %d\n",crow,ccol);

//write c from thread and zero op again
for(int i=0;i<crow;i++){
  for(int j=0;j<ccol;j++){
  //  printf("%d ",matrix[i][j]);
    fprintf(matc,"%d ",matrix[i][j]);
    matrix[i][j]=0;
  }//printf("\n");
  fprintf(matc,"\n");
}

 printf("%d Thread taken %lu MicroSec\n",crow*ccol ,stop.tv_usec - start.tv_usec);
// fprintf(matc,"%d Thread taken %lu MicroSec\n",crow*ccol ,stop.tv_usec - start.tv_usec);

 //start time for row thr
 gettimeofday(&start, NULL);
 pthread_t threads[crow];
 for(int i=0;i<crow;i++){
  if( pthread_create(&threads[i],NULL,RowMul,(void *)i) )
    printf("can not create thread\n");
 }
 for(int i=0;i<crow;i++){
  pthread_join(threads[i],NULL);
 }
 gettimeofday(&stop, NULL);

 fprintf(matc,"\nThread Row\nrow= %d col= %d\n",crow,ccol);
 //printf("\nThread Row\nrow= %d col= %d\n",crow,ccol);

for(int i=0;i<crow;i++){
  for(int j=0;j<ccol;j++){
 //   printf("%d ",matrix[i][j]);
    fprintf(matc,"%d ",matrix[i][j]);
  }//printf("\n");
   fprintf(matc,"\n");
}

 printf("%d Thread taken %lu MicroSec\n",crow ,stop.tv_usec - start.tv_usec);
 //fprintf(matc,"%d Thread taken %lu MicroSec\n",crow ,stop.tv_usec - start.tv_usec);
 fclose(matc);

 return 0;
}


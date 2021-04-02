#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

long usecs (){
  struct timeval t;

  gettimeofday(&t,NULL);
  return t.tv_sec*1000000+t.tv_usec;
}

double dnorm2_seq(double *x, int n);
double dnorm2_par_red(double *x, int n);
double dnorm2_par_nored(double *x, int n);


int main(int argc, char *argv[]){

  int n, i;
  double *x;
  double n2_seq, n2_par_red, n2_par_nored;
  long    t_start,t_end;

  
  if(argc!=2){
    printf("Wrong number of arguments.\n Usage:\n\n\
./main n \n\n where n is the size of the vector x whose 2-norm has to be computed.\n");
    return 1;
  }

  
  sscanf(argv[1],"%d",&n);

  
  x = (double*)malloc(sizeof(double)*n);

  for(i=0; i<n; i++)
    x[i] = ((double) rand() / (RAND_MAX));


  printf("\n================== Sequential version ==================\n");
  t_start = usecs();
  n2_seq       = dnorm2_seq(x, n);
  t_end = usecs();
  printf("Time (msec.) : %7.1f\n",(t_end-t_start)/1e3);
  printf("Computed norm is: %10.3lf\n",n2_seq);

  printf("\n\n=========== Parallel version with reduction  ===========\n");
  t_start = usecs();
  n2_par_red   = dnorm2_par_red(x, n);
  t_end = usecs();
  printf("Time (msec.) : %7.1f\n",(t_end-t_start)/1e3);
  printf("Computed norm is: %10.3lf\n",n2_par_red);


  printf("\n========== Parallel version without reduction ==========\n");
  t_start = usecs();
  n2_par_nored = dnorm2_par_nored(x, n);
  t_end = usecs();
  printf("Time (msec.) : %7.1f\n",(t_end-t_start)/1e3);
  printf("Computed norm is: %10.3lf\n",n2_par_nored);


  printf("\n\n");
  if(fabs(n2_seq-n2_par_red)/n2_seq > 1e-10) {
    printf("The parallel version with reduction is numerically wrong! \n");
  } else {
    printf("The parallel version with reduction is numerically okay!\n");
  }
  
  if(fabs(n2_seq-n2_par_nored)/n2_seq > 1e-10) {
    printf("The parallel version without reduction is numerically wrong!\n");
  } else {
    printf("The parallel version without reduction is numerically okay!\n");
  }
  
  return 0;

}



double dnorm2_seq(double *x, int n){
  int i;
  double res;

  res = 0.0;

  for(i=0; i<n; i++)
    res += x[i]*x[i];

  return sqrt(res);
  
}

double dnorm2_par_red(double *x, int n){
  int i;
  double res;

  res = 0.0;
  #pragma omp parallel for reduction(+:res)
  for(i=0; i<n; i++)
    res += x[i]*x[i];

  return sqrt(res);
  
}

double dnorm2_par_nored(double *x, int n){
  int i, iam;
  double res,myres;

  res = 0.0;
  #pragma omp parallel private(i) firstprivate(myres)
  {
    #pragma omp for 
    for(i=0; i<n; i++)
      myres+= x[i]*x[i]; 

    #pragma omp critical
    {
      res += myres;
    }
  }
  return sqrt(res);
  
}

/* Analyse et comparaison :
--> Pour l'exemple de n=5000000
    On remarque que la différence entre Tseq et Tpara es grande pour 4 threads que pour 2 threads sachant que dans 
    les deux cas on Tseq > Tpara , orcette différence devient plus grande si on ajoute des threads.

    On remarque aussi que la différence entre Tprared et Tparanonred sont proche meme si Tparared > Tparannred.

    --> Voici l'exemple :
    NUM_THREADS=2
    ================== Sequential version ==================
    Time (msec.) :    59.1
    Computed norm is:   4082.407


    =========== Parallel version with reduction  ===========
    Time (msec.) :    29.9
    Computed norm is:   4082.407

    ========== Parallel version without reduction ==========
    Time (msec.) :    29.8
    Computed norm is:   4082.407


    NUM_THREADS=4
    ================== Sequential version ==================
    Time (msec.) :    59.2
    Computed norm is:   4082.407


    =========== Parallel version with reduction  ===========
    Time (msec.) :    16.4
    Computed norm is:   4082.407

    ========== Parallel version without reduction ==========
    Time (msec.) :    16.0
    Computed norm is:   4082.407
*/



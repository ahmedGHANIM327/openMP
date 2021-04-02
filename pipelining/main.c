#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include "omp.h"
#include "aux.h"

void pipeline(data *datas, resource *resources, int ndatas, int nsteps);


int main(int argc, char **argv){
  int   n, i, s, d, ndatas, nsteps;
  long  t_start, t_end;
  data *datas;
  resource *resources;

  // Command line arguments
  if ( argc == 3 ) {
    ndatas  = atoi(argv[1]);    /* num of datas */
    nsteps  = atoi(argv[2]);    /* num of steps */
  } else {
    printf("Usage:\n\n ./main ndatas nsteps\n where ndatas is the number of data and nsteps the number of steps.\n");
    return 1;
  }

  init_data(&datas, &resources, ndatas, nsteps);
  
  /* Process all the data */
  t_start = usecs();
  pipeline(datas, resources, ndatas, nsteps);
  t_end = usecs();
  printf("Execution   time    : %8.2f msec.\n",((double)t_end-t_start)/1000.0);

  
  check_result(datas, ndatas, nsteps);
  
  return 0;
  
}

void pipeline(data *datas, resource *resources, int ndatas, int nsteps){
 
  int d, s;
  omp_lock_t * locks = (omp_lock_t *) malloc(nsteps*sizeof(omp_lock_t));
  
  for(int i = 0; i < nsteps ; i++){
    omp_init_lock(&locks[i]);
  }
  
  #pragma omp parallel for private(s)
  /* Loop over all the data */
  for (d=0; d<ndatas; d++){
    /* Loop over all the steps */
    for (s=0; s<nsteps; s++){
      omp_set_lock(&locks[s]);
      process_data(datas, d, s, &(resources[s]));
      omp_unset_lock(&locks[s]);
    }
  }
    
  for(int i = 0; i < nsteps ; i++){
    omp_destroy_lock(&locks[i]);
  }
}

/*

$ export OMP_NUM_THREADS=1
$ ./main 100 20
Execution   time    : 20000.03 msec.
The result is correct!!!

=================================================

$ export OMP_NUM_THREADS=2
$ ./main 100 20
Execution   time    : 10010.15 msec.
The result is correct!!!

=================================================

$ export OMP_NUM_THREADS=4
$ ./main 100 20
Execution   time    :  5149.83 msec.
The result is correct!!!

--> Conclusion :
  On a utilisé le lock pour qu'on aura pas le conflit d'utilisation du meme resource en meme temps
  et pour plus de vitesse , on a pas utilisé un seul lock pour que si un processus veu utilisé un différent ressource 
  peut faire cela meme si un autre ressource et déja verouié.

  Et on remarque comme dans l'exemple la diminution au niveau du temps d'execution.

*/
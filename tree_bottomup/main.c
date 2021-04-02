#include "aux.h"


void bottom_up(int nleaves, struct node **leaves, int nnodes);

int main(int argc, char **argv){
  long   t_start, t_end;
  int    nnodes, nleaves;
  struct node **leaves;

  // Command line argument: number of nodes in the tree
  if ( argc == 2 ) {
    nnodes = atoi(argv[1]); 
  } else {
    printf("Usage:\n\n ./main n\n\nwhere n is the number of nodes in the tree.\n");
    return 1;
  }

  printf("\nGenerating a tree with %d nodes\n\n",nnodes);
  generate_tree(nnodes, &leaves, &nleaves);
  
  t_start = usecs();
  bottom_up(nleaves, leaves, nnodes);
  t_end = usecs();
  
  printf("Parallel time : %8.2f msec.\n\n",((double)t_end-t_start)/1000.0);

  check_result();
  
}
  

/* You can change the number and type of arguments if needed.     */
/* Just don't forget to update the interface declaration above.   */
void bottom_up(int nleaves, struct node **leaves, int nnodes){

  /* Implement this routine */
  int i, check_noeud;
  struct node *noeud_actuelle; // element pour traiter la noeud
  int * processed_nodes = (int *) malloc(nnodes*sizeof(int));
  
  // on initialise les noeuds
  for (int j = 0; j < nnodes; j++){
    processed_nodes[j] = 0;
  }
 
  //Oncommence le parallelisme
  #pragma omp parallel private(check_noeud,noeud_actuelle)
  {
    #pragma omp for
    for (i = 0; i< nleaves; i++){
      noeud_actuelle = leaves[i];
      //On traite la noeud et ses parents jusqu'à arriver au dernier parent.
      while (noeud_actuelle != NULL){
        check_noeud = 0;
        #pragma omp critical
        {
          if (processed_nodes[(noeud_actuelle->id) - 1] == 0){
            processed_nodes[(noeud_actuelle->id) - 1] = 1;
            check_noeud = 1;
          }
        }
    
        if (check_noeud == 1) {
          process_node(noeud_actuelle);
        }
        noeud_actuelle = noeud_actuelle->parent;
        // A la fin on passe au parent
      }
    }
  }

}

/*
    L'utilisation du du parallelisme dans le parcours des noeuds , puisque on utilise des boucle
  et on a utiliser aussi critical , puisque dans cette region puisque d'avoir des accés simultanés au
  prossed_nodes .
*/
    





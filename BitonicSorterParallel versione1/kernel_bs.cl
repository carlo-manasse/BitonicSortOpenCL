
__kernel void BitonicSortParallel(__global int *a,__global int *b) {
 
	int id = get_global_id(0);
	int num_thread=get_global_size(0);//thread number

	int thread_work=(int)(b[0]/(num_thread*2));// su quanti elementi un thread deve lavorare	
	

	int i;
	int j;
	int l;
  
	for (i=0;i<b[1];i++){
		
		for (j=i;j>-1;j--){	

			int dist = 1;

			int p;
			//dist=2^(j)
			for(p=0;p<j;p++){
				dist=dist*2;
					
			}	

			//int dist=(int)pow((float) 2,(float) j);//distanza per comparazioni
		
			for(l=0;l<thread_work;l++){			
				int map_id=num_thread*l+id;//funzione per mappare l'id. se il numero di thread è uguale alla metà della lunghezza dell'array la map è l'identità. se sono di meno thread_work > 1 e ogni thread lavora su più blocchi  	
			
			int k=map_id+((int)map_id/dist)*dist;//indice dell'elemento da confrontare con a[k+dist]
			
			//printf(" sono thread %d , mappato con %d. Al passo i=%d,j=%d -> k vale %d \n",id,map_id,i,j,k);

			//calcolo della parità del blocco cui appartiene k

			int z=1;
			//z=2^(i+1)
			for(p=0;p<i+1;p++){
				z=z*2;
					
			}

			int r=(int) (k/z); 
			//int r=k/((int)pow((float) 2,(float) (i+1)));
			int pari=r%2;

			//swap degli elementi se xor==1
			if( ((int)(a[k]>a[k+dist]) + pari)==1 ) {	
				int tmp=a[k];
				a[k]=a[k+dist];
				a[k+dist]=tmp;
			}

			}

			//aspetta che gli altri work-item abbiano terminato l'esecuzione
			barrier(CLK_LOCAL_MEM_FENCE);
			//barrier(CLK_GLOBAL_MEM_FENCE);
		}

	}

}

__kernel void BitonicSortParallel(__global int *a,__global int *b) {
 
	int id = get_global_id(0);
	int i=b[2];
	int j=b[3];

			int dist = 1;

			int p;
			//dist=2^(j)
			for(p=0;p<j;p++){
				dist=dist*2;
					
			}	
			
			int k=id+((int)id/dist)*dist;//indice dell'elemento da confrontare con a[k+dist]
			
			//calcolo della parità del blocco cui appartiene k

			int z=1;
			//z=2^(i+1)
			for(p=0;p<i+1;p++){
				z=z*2;
					
			}

			int r=(int) (k/z); 
			int pari=r%2;

			//swap degli elementi se xor==1
			if( ((int)(a[k]>a[k+dist]) + pari)==1 ) {	
				int tmp=a[k];
				a[k]=a[k+dist];
				a[k+dist]=tmp;
			}

}

#define X 500
#define Y 500 //x
#define mat_ave 1/(X*Y)
#define mini 0.005
#define big 0.01
#define max_try 50000
#define sum_latteice 800000
#define ee 1.0E-12
#define FP_TYPE float
#define datasize X*Y*sizeof(float)

__kernel void Heat(__global FP_TYPE* temp, __global FP_TYPE* temp_save ,__global FP_TYPE *delta) {
    __global  FP_TYPE sum_delta;
    __global int i = get_global_id(0);
    __global int j = get_global_id(1);
    __global int N = X;
   
  if((i < X) && (j < Y)){

    const int id = (j + 1)*(N + 2) + (i + 1);

       temp_save[id] = 0.25f*(rhs[id]
		      + temp[i - (N+2)]
		      + temp[i + (N+2)]
		      + temp[i - 1]
		      + temp[i + 1]);
  }
}

	
}
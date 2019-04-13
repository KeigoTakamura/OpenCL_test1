#include<stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<time.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#define MAX_SOURCE_SIZE (0x100000)

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

int  main(){
    time_t time_test = time(NULL);
    FP_TYPE *temp_lay;  //計算領域
    FP_TYPE *temp_lay_save; //保存領域
    FP_TYPE *sum_delta;

    const char filename[] ="./karnel1.cl";
    cl_platform_id  platform_id1=NULL; 
    cl_device_id device1 = NULL; //デバイス格納
    cl_uint ret_num_plotform;//プラットフォーム格納 
    cl_uint ret_num_device;
    cl_int ret; //ret

    cl_mem memobj1 = NULL;//計算領域
    cl_mem memobj2 = NULL;//保存領域
    cl_mem memobj3 = NULL;//なんだろうね・・

    cl_context context=NULL;//コンテキストの作成
    cl_command_queue command_queue1=NULL;//コマンドキューの新規作成
    
    cl_kernel main_kernel=NULL;//kernelオブジェクト
    cl_program  heat_program = NULL;//kernelコード

    ret = clGetPlatformIDs(1,&platform_id1,&ret_num_plotform);//プラットフォーム設定
    ret = clGetDeviceIDs(platform_id1,CL_DEVICE_TYPE_GPU,1,&device1,&ret_num_device);//デバイス設定
    
    context = clCreateContext(NULL,1,&device1,NULL,NULL,&ret); //
    command_queue1 = clCreateCommandQueue(context, device1, 0, &ret);//こまんどきゅーの作成（推奨されてないらしくて草）

    FILE *kernel_code1;
    FILE *fp1;
    size_t source_size;
    char *source_str;
    
    kernel_code1 = fopen(filename,"r");
    if(NULL==kernel_code1){
        fprintf(stderr,"Failed to load kernel Code!!!!!\n");
        exit(1);
    }

    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread (source_str,1,MAX_SOURCE_SIZE,kernel_code1);
    fclose(kernel_code1);

    memobj1 = clCreateBuffer(context, CL_MEM_READ_WRITE, datasize, NULL, &ret);//計算領域用バッファオブジェクト
    memobj2 = clCreateBuffer(context, CL_MEM_READ_WRITE, datasize, NULL, &ret);//保存領域用バッファオブジェクト
    memobj3 = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(FP_TYPE), NULL, &ret);//計算処理用

    temp_lay=(FP_TYPE*)malloc(X*Y*sizeof(FP_TYPE));
    temp_lay_save=(FP_TYPE*)malloc(X*Y*sizeof(FP_TYPE));

  for(int i = 0; i < X; i++)  //領域初期化
    {
        for(int j = 0; j < Y; j++)
        {
            temp_lay[i*X +j]  = 0.0;
            temp_lay_save[i*X +j] = 0.0;
        }
    }
    
    for(int i=0;i < X; i++){ //上底部分条件設定

        temp_lay[i]=1.0;
    }

    for(int k = 0; k < Y; k++)//左右部分条件設定
    {
        temp_lay[X*k]=-1.0;
        temp_lay[X*k+X-1]=-1.0;
    }

    for(int t = 0; t < Y; t++)//下底部分設定
    {
       temp_lay[X*(Y-1)+t]=-1.0;
    }

    ret = clEnqueueWriteBuffer(command_queue1,memobj1,CL_TRUE,0,datasize,temp_lay,0,NULL,NULL);  //バッファにデーター転送 
    ret = clEnqueueWriteBuffer(command_queue1,memobj2,CL_TRUE,0,datasize,temp_lay_save,0,NULL,NULL);

    heat_program = clCreateProgramWithSource(context, 1,(const char**)&source_str,(const size_t *)&source_size,&ret);//プログラムオブジェクト
    ret  = clBuildProgram(heat_program, 1, &device1, NULL, NULL, NULL);//オフラインコンパイル用ビルド
    main_kernel = clCreateKernel(heat_program, "Heat", &ret);

    ret = clSetKernelArg(main_kernel,0,sizeof(cl_mem),(void *)&memobj1);//カーネル関数第一引数（計算領域用）
    ret = clSetKernelArg(main_kernel,1,sizeof(cl_mem),(void *)&memobj2);//カーネル関数第二引数(保存領域用)
     ret = clSetKernelArg(main_kernel,1,sizeof(cl_mem),(void *)&memobj3);
    ret = clEnqueueTask(command_queue1,main_kernel,0,NULL,NULL);//これも非推奨らしい、カーネル関数実行
    
    ret = clEnqueueReadBuffer(command_queue1,memobj2,CL_TRUE,0,datasize,temp_lay_save,0,NULL,NULL);



    printf("time_test %ld \n",time(NULL) - time_test);
	fp1=fopen("date_3dmap_2d.txt", "w+");

    	for (size_t i = 0; i < Y; i++)
	{
		for (size_t t = 0; t < X; t++)
		{
			fprintf(fp1, "%lf %lf %lf \n", (double)i*big, (double)t*big, temp_lay[i*X+t] );
			//fprintf(fp1, "%lf %lf %lf \n", (double)i*big, (double)t*big, temp_lay[i][t]);
		}
		fprintf(fp1, "\n");
	}


  fclose(fp1);
  ret = clFlush(command_queue1);
  ret = clFinish(command_queue1);
  ret = clReleaseKernel(main_kernel);
  ret = clReleaseProgram(heat_program);
  ret = clReleaseMemObject(memobj1);
  ret = clReleaseMemObject(memobj2);
  ret = clReleaseCommandQueue(command_queue1);
  ret = clReleaseContext(context);
  free(source_str);




return 0;
}
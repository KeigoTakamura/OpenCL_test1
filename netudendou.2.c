//C言語で実装されてる熱伝導計算　ヤコビ法に書き換えるンゴ

#include<stdio.h>
#include<math.h>
#include<time.h>
#include<omp.h>
#define X 400
#define Y 200 //x
#define mat_ave 1/(X*Y)
#define mini 0.005
#define big 0.01
#define max_try 50000
#define sum_latteice 800000
#define ee 1.0E-12


int main() {
	double temp_lay[Y][X];
	double temp_lay2[Y][X];
	double wh = 1.0;
	double lattice_size = mini;
	double sum_delta = 0.0;
	double delta = 0.0;
	int i,t,cout;
	FILE *fp1;
	FILE *temp_10;
	FILE *temp_04;
	FILE *temp_16;
	//FILE *fp2;

	time_t time_test = time(NULL);

	for ( i = 0; i < Y; i++)
	{
		for ( t = 0; t < X; t++)
		{
			temp_lay[i][t] = 0.00;
			temp_lay2[i][t] = 0.00;
		}
	}



	for ( i = 0; i < X; i++)
	{
		temp_lay[0][i] = 1.0;
		temp_lay[Y - 1][i] = -1.0;
	}
	for ( i = 1; i < Y; i++)
	{
		temp_lay[i][0] = -1.0;
		temp_lay[i][X - 1] = -1.0;
	}
	
	for ( cout = 0; cout < 50000; ++cout)
	{
		sum_delta = 0.0;
		#pragma omp parallel for 
		for ( i = 1; i < Y-1; ++i)
		{
			for ( t= 1; t < X-1 ; ++t)
			{	
				delta = temp_lay[i][t];
				temp_lay2[i][t] = (temp_lay[i][t - 1] + temp_lay[i - 1][t] + temp_lay[i][t + 1] + temp_lay[i + 1][t])*0.25;
				sum_delta += fabs(temp_lay2[i][t]-delta)*fabs(temp_lay2[i][t]-delta);
			}
		}
		for ( i = 1;  i < Y-1; ++i)
		{
			for ( t = 1; t < X-1 ; ++t)
			{	
					temp_lay[i][t] = temp_lay2[i][t];
			}
		}

		if(sum_delta*mat_ave < ee){
			printf("end cal\n");
			break;
		}
		
	}
	
	printf("time_test %ld \n",time(NULL) - time_test);
	fp1=fopen("date_3dmap_2d_skip_0.05.txt", "w+");
	temp_10=fopen("data_2_10.txt", "w+");
	temp_04=fopen("data_2_04.txt", "w+");
	temp_16=fopen("data_2_16.txt", "w+");

	for (size_t i = 0; i < Y; i++)
	{
		for (size_t t = 0; t < X; t++)
		{
			fprintf(fp1, "%lf %lf %lf \n", (double)i*big, (double)t*big, temp_lay[i][t]);
			//fprintf(fp1, "%lf %lf %lf \n", (double)i*big, (double)t*big, temp_lay[i][t]);
		}
		fprintf(fp1, "\n");
	}

		for(int i = 0 ;i < Y;i++){
		fprintf(temp_10,"%lf %lf\n",(double)i*big,temp_lay[i][(int)(1.0/big)]);
		fprintf(temp_16,"%lf %lf\n",(double)i*big,temp_lay[i][(int)(1.6/big)]);
		fprintf(temp_04,"%lf. %lf\n",(double)i*big,temp_lay[i][(int)(0.4/big)]);
	}

	fclose(temp_04);
	fclose(temp_16);
	fclose(temp_10);
	fclose(fp1);
    return 0;
}
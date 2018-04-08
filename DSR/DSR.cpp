// DSR.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <samplerate.h>

using namespace std;

#define INP_SAMPLE_RATE 44100
#define OUT_SAMPLE_RATE 8000

void dsr()
{
	//************INITIALIZATION*********
	int error = 0;
	const double cratio = (double)OUT_SAMPLE_RATE / (double)INP_SAMPLE_RATE;
	//const double cratio = (double)INP_SAMPLE_RATE / (double)OUT_SAMPLE_RATE;

	if (src_is_valid_ratio(cratio)) {
		printf("## ratio %lf is fine\n", cratio);
	}
	//TODO: see different type of configuration like SRC_SINC_FASTEST
	SRC_STATE* src = src_new(SRC_SINC_FASTEST, 1, &error);
	if (src == NULL) {
		printf("## src_new() failed : %s.\n\n", src_strerror(error));
	}
	//call this to reset int src_reset (SRC_STATE *state) ;

	//************PROCESSING***************

	FILE *fin = fopen("inp.pcm", "rb"), *fout = fopen("out.pcm", "wb");

	if (!fin || !fout) {
		printf("## file not found..");
		return;
	}

	short sarray[10000];
	float finarray[10000], foutarray[10000];

	const int in_size = INP_SAMPLE_RATE / 10;

	SRC_DATA src_data;
	src_data.data_in = finarray;
	src_data.data_out = foutarray;
	src_data.end_of_input = 0;
	src_data.input_frames = in_size;
	src_data.output_frames = 10000;
	src_data.src_ratio = cratio;
	int c = 0;
	while (fread(sarray, 2, in_size, fin) == in_size) {
		src_short_to_float_array(sarray, finarray, in_size);
		//if (c++ > 50)
		//	src_data.end_of_input = 1;
		int ret = src_process(src, &src_data);
		//printf("## src_process returns: %d, in-done:%d out:%d", ret, (int)src_data.input_frames_used, (int)src_data.output_frames_gen);
		src_float_to_short_array(foutarray, sarray, src_data.output_frames_gen);
		fwrite(sarray, 2, src_data.output_frames_gen, fout);
	}
	//*************DESTRUCTION********************
	if (src_delete(src) != NULL){
		printf("## src_delete failed.");
	}
	printf("******************Converstion completed*****************\n");
	fclose(fin);
	fclose(fout);
}
int main()
{
	dsr();
	return 0;
}


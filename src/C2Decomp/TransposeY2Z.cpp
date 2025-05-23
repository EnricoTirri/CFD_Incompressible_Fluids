#include "C2Decomp.hpp"

void C2Decomp::transposeY2Z(C2D_DTYPE *src, C2D_DTYPE *dst){

    int s1, s2, s3, d1, d2, d3;

    s1 = decompMain.ysz[0];
    s2 = decompMain.ysz[1];
    s3 = decompMain.ysz[2];

    d1 = decompMain.zsz[0];
    d2 = decompMain.zsz[1];
    d3 = decompMain.zsz[2];

    memSplitYZ(src, s1, s2, s3, work1_r, dims[1], decompMain.y2dist);

    MPI_Alltoallv(work1_r, decompMain.y2cnts, decompMain.y2disp, C2D_MPI_DTYPE,
		  dst,     decompMain.z2cnts, decompMain.z2disp, C2D_MPI_DTYPE,
		  DECOMP_2D_COMM_ROW);


}

void C2Decomp::transposeY2Z_MajorIndex(C2D_DTYPE *src, C2D_DTYPE *dst){

    int s1, s2, s3, d1, d2, d3;

    s1 = decompMain.ysz[0];
    s2 = decompMain.ysz[1];
    s3 = decompMain.ysz[2];

    d1 = decompMain.zsz[0];
    d2 = decompMain.zsz[1];
    d3 = decompMain.zsz[2];

    memSplitYZ_YMajor(src, s1, s2, s3, work1_r, dims[1], decompMain.y2dist);

    MPI_Alltoallv(work1_r, decompMain.y2cnts, decompMain.y2disp, C2D_MPI_DTYPE,
		  work2_r,     decompMain.z2cnts, decompMain.z2disp, C2D_MPI_DTYPE,
		  DECOMP_2D_COMM_ROW);

    //Just do the transpose here...
    for(int kp = 0; kp < d3; kp++){
	for(int jp = 0; jp < d2; jp++){
	    for(int ip = 0; ip < d1; ip++){
		int ii  = kp*d2*d1 + jp*d1 + ip;
		int iip = jp*d3*d1 + ip*d3 + kp;
		dst[iip] = work2_r[ii]; 
	    }
	}
    } 

}

void C2Decomp::transposeY2Z_Start(MPI_Request &handle, C2D_DTYPE *src, C2D_DTYPE *dst, C2D_DTYPE *sbuf, C2D_DTYPE *rbuf){

    int s1, s2, s3;

    s1 = decompMain.ysz[0];
    s2 = decompMain.ysz[1];
    s3 = decompMain.ysz[2];

    memSplitYZ(src, s1, s2, s3, sbuf, dims[1], decompMain.y2dist);

    MPI_Ialltoallv(sbuf, decompMain.y2cnts, decompMain.y2disp, C2D_MPI_DTYPE,
                   rbuf, decompMain.z2cnts, decompMain.z2disp, C2D_MPI_DTYPE,
                   DECOMP_2D_COMM_ROW, &handle);


}

void C2Decomp::transposeY2Z_Wait(MPI_Request &handle, C2D_DTYPE *src, C2D_DTYPE *dst, C2D_DTYPE *sbuf, C2D_DTYPE *rbuf){

    int d1, d2, d3;
    MPI_Status status;

    d1 = decompMain.zsz[0];
    d2 = decompMain.zsz[1];
    d3 = decompMain.zsz[2];

    MPI_Wait(&handle, &status);

    memcpy(dst, rbuf, d1*d2*d3*sizeof(double));

}

void C2Decomp::transposeY2Z_MajorIndex_Start(MPI_Request &handle, C2D_DTYPE *src, C2D_DTYPE *dst, C2D_DTYPE *sbuf, C2D_DTYPE *rbuf){

    int s1, s2, s3;

    s1 = decompMain.ysz[0];
    s2 = decompMain.ysz[1];
    s3 = decompMain.ysz[2];

    memSplitYZ_YMajor(src, s1, s2, s3, sbuf, dims[1], decompMain.y2dist);

    MPI_Ialltoallv(sbuf, decompMain.y2cnts, decompMain.y2disp, C2D_MPI_DTYPE,
                   rbuf, decompMain.z2cnts, decompMain.z2disp, C2D_MPI_DTYPE,
                   DECOMP_2D_COMM_ROW, &handle);


}

void C2Decomp::transposeY2Z_MajorIndex_Wait(MPI_Request &handle, C2D_DTYPE *src, C2D_DTYPE *dst, C2D_DTYPE *sbuf, C2D_DTYPE *rbuf){

    int d1, d2, d3;
    MPI_Status status;

    d1 = decompMain.zsz[0];
    d2 = decompMain.zsz[1];
    d3 = decompMain.zsz[2];

    MPI_Wait(&handle, &status);

    //Just do the transpose here...
    for(int kp = 0; kp < d3; kp++){
        for(int jp = 0; jp < d2; jp++){
            for(int ip = 0; ip < d1; ip++){
                int ii  = kp*d2*d1 + jp*d1 + ip;
                int iip = jp*d3*d1 + ip*d3 + kp;
                dst[iip] = rbuf[ii];
            }
        }
    }

}

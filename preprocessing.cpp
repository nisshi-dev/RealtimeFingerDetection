#include "opencv2/opencv.hpp"
#include "preprocessing.hpp"

using namespace cv;

Mat RGBtoHSV(Mat img){
	int width = img.cols;
	int height = img.rows;
	float r, g, b;
	float h, s, v;
	float _max, _min;

	Mat hsv = Mat::zeros(height, width, CV_32FC3);

	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			r = (float)img.at<Vec3b>(y, x)[2] / 255;
			g = (float)img.at<Vec3b>(y, x)[1] / 255;
			b = (float)img.at<Vec3b>(y, x)[0] / 255;

			_max = fmax(r, fmax(g, b));
			_min = fmin(r, fmin(g, b));

			if(_max == _min){
				h = 0;
			} else if (_min == b) {
				h = 60 * (g - r) / (_max - _min) + 60;
			} else if (_min == r) {
				h = 60 * (b - g) / (_max - _min) + 180;
			} else if (_min == g) {
				h = 60 * (r - b) / (_max - _min) + 300;
			}

			s = _max - _min;

			v = _max;

			hsv.at<Vec3f>(y, x)[0] = h;
			hsv.at<Vec3f>(y, x)[1] = s;
			hsv.at<Vec3f>(y, x)[2] = v;
		}
	}

	return hsv;
}

Mat RGBtoGray(Mat frame) {
	Mat frame_gray = Mat::zeros(Size(frame.cols, frame.rows), CV_8UC1);

	for(int y = 0; y < frame.rows; y++){
                for(int x = 0; x < frame.cols; x++){
                        frame_gray.at<unsigned char>(y,x) =
								0.2126*frame.at<cv::Vec3b>(y,x)[2]
                                + 0.7152*frame.at<cv::Vec3b>(y,x)[1]
                                + 0.0722*frame.at<cv::Vec3b>(y,x)[0];
                }
    }

	return frame_gray;
}

Mat GenerateMask(Mat frame){
	//#define MIN_HSVCOLOR cv::Scalar(0, 60, 80)
	//#define MAX_HSVCOLOR cv::Scalar(10, 160, 240)
	Mat frame_out = Mat::zeros(Size(frame.cols, frame.rows), CV_8UC1);

	int width = frame.cols;
	int height = frame.rows;

	float h, s, v;

	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			if(y >= 60 && y<=420 && x >=140 && x <=500){
				h = frame.at<Vec3f>(y, x)[0];
				s = frame.at<Vec3f>(y, x)[1];
				v = frame.at<Vec3f>(y, x)[2];

				//if(h >= 0 && h <= 10 && s >= 60 && s <= 160 && v >= 80 && v <= 240){
				if(h <= 50){
					frame_out.at<unsigned char>(y, x) = 255;
				}
			}
		}
	}

	return frame_out;
}

Mat CalcGravity(Mat frame) {
	int sx = 0;
	int sy = 0;
	int mm = 0;
	int width = frame.cols;
	int height = frame.rows;

	int x, y;
	float gpx, gpy;

	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			if (frame.at<unsigned char>(y, x) == 255) {
				sx += (float)x;
				sy += (float)y;
				++mm;
			}
		}
	}

	gpx = (float)sx/(float)mm;
	gpy = (float)sy/(float)mm;
	printf( "Total = %d, Gravity Center( x, y) = %f, %f\n", mm, gpx, gpy );

	// 画像，円の中心座標，半径，色，線太さ，種類
	circle(frame, Point((int)gpx, (int)gpy), 10, 100, 3, 4);

	return frame;
}

Mat Sikisou_tyuusyutu(Mat frame, Mat frame_out, int down1, int down2){
	int x, y;
	int d1 = down1, d2 = down2;
	int H[frame.rows][frame.cols];
	if(d1 > d2){
                int t = d1;
                d1 = d2;
                d2 = t;
        }
	for(y = 0; y < frame.rows; y++){
                for(x = 0; x < frame.cols; x++){
                        if(frame.at<cv::Vec3b>(y,x)[2] >= frame.at<cv::Vec3b>(y,x)[1] &&
                           frame.at<cv::Vec3b>(y,x)[2] >= frame.at<cv::Vec3b>(y,x)[0]){
                                if(frame.at<cv::Vec3b>(y,x)[1] > frame.at<cv::Vec3b>(y,x)[0]){
                                        H[y][x] = (int)floor(60.0*(frame.at<cv::Vec3b>(y,x)[1]-frame.at<cv::Vec3b>(y,x)[0])/
                                                (frame.at<cv::Vec3b>(y,x)[2]-frame.at<cv::Vec3b>(y,x)[0]));
                                }else{
                                        H[y][x] = (int)floor(60.0*(frame.at<cv::Vec3b>(y,x)[1]-frame.at<cv::Vec3b>(y,x)[0]*1.0)/
                                                (frame.at<cv::Vec3b>(y,x)[2]-frame.at<cv::Vec3b>(y,x)[1]));
                                }
                        }else if(frame.at<cv::Vec3b>(y,x)[1] >= frame.at<cv::Vec3b>(y,x)[2] &&
                           frame.at<cv::Vec3b>(y,x)[1] >= frame.at<cv::Vec3b>(y,x)[0]){
                                if(frame.at<cv::Vec3b>(y,x)[2] > frame.at<cv::Vec3b>(y,x)[0]){
                                        H[y][x] = (int)floor(60.0*(frame.at<cv::Vec3b>(y,x)[0]-frame.at<cv::Vec3b>(y,x)[2]*1.0)/
                                                (frame.at<cv::Vec3b>(y,x)[1]-frame.at<cv::Vec3b>(y,x)[0]));
                                }else{
                                        H[y][x] = (int)floor(60.0*(frame.at<cv::Vec3b>(y,x)[0]-frame.at<cv::Vec3b>(y,x)[2]*1.0)/
                                                (frame.at<cv::Vec3b>(y,x)[1]-frame.at<cv::Vec3b>(y,x)[2]));
                                }
                        }else if(frame.at<cv::Vec3b>(y,x)[0] >= frame.at<cv::Vec3b>(y,x)[2] &&
                           frame.at<cv::Vec3b>(y,x)[0] >= frame.at<cv::Vec3b>(y,x)[1]){
                                if(frame.at<cv::Vec3b>(y,x)[2] > frame.at<cv::Vec3b>(y,x)[1]){
                                        H[y][x] = (int)floor(60.0*(frame.at<cv::Vec3b>(y,x)[2]-frame.at<cv::Vec3b>(y,x)[1]*1.0)/
                                                (frame.at<cv::Vec3b>(y,x)[0]-frame.at<cv::Vec3b>(y,x)[1]));
                                }else{
                                        H[y][x] = (int)floor(60.0*(frame.at<cv::Vec3b>(y,x)[2]-frame.at<cv::Vec3b>(y,x)[1]*1.0)/
                                                (frame.at<cv::Vec3b>(y,x)[0]-frame.at<cv::Vec3b>(y,x)[2]));
                                }
                        }
                        if(H[y][x] < 0){
                                H[y][x] += 360;
                                if(H[y][x] < 0){
                                        H[y][x] = 360;
                                }
                        }
		}
	}
	for(y = 0; y < frame.rows; y++){
                for(x = 0; x < frame.cols; x++){
                        if(H[y][x] >= d1 && H[y][x] <= d2)	H[y][x] = 360;
                }
        }
	for(y = 0; y < frame.rows; y++){
                for(x = 0; x < frame.cols; x++){
                        if(H[y][x] == 360){
                                frame_out.at<cv::Vec3b>(y,x)[2] = 0;
				frame_out.at<cv::Vec3b>(y,x)[1] = 0;
				frame_out.at<cv::Vec3b>(y,x)[0] = 0;
			}else{
				frame_out.at<cv::Vec3b>(y,x)[2] = frame.at<cv::Vec3b>(y,x)[2];
				frame_out.at<cv::Vec3b>(y,x)[1] = frame.at<cv::Vec3b>(y,x)[1];
				frame_out.at<cv::Vec3b>(y,x)[0] = frame.at<cv::Vec3b>(y,x)[0];
			}
                }
    }

	return frame_out;
}

Mat kido_tyuusyutu(Mat frame, Mat frame_out, int down1, int down2){
	int x, y;
	int d1 = down1, d2 = down2;

	if(d1 > d2){
                int t = d1;
                d1 = d2;
                d2 = t;
    }

	for(y = 0; y < frame.rows; y++){
                for(x = 0; x < frame.cols; x++){
                        if(frame.at<unsigned char>(y,x) < d1){
                                frame_out.at<unsigned char>(y,x) = 0;
						}
                        else if(frame.at<unsigned char>(y,x) > d2){
                                frame_out.at<unsigned char>(y,x) = 0;
						}
                        else{
                                frame_out.at<unsigned char>(y,x) = frame.at<unsigned char>(y,x);
						}
				}
    }

	return frame_out;
}

Mat Nichika(Mat frame, int Shikiichi){
	int x,y;
	Mat frame_out = frame;

	for(y = 0; y < frame.rows; y++){
		for(x = 0; x < frame.cols; x++){
			if(frame.at<unsigned char>(y,x) > Shikiichi)
				frame_out.at<unsigned char>(y,x) = 255;
			else
				frame_out.at<unsigned char>(y,x) = 0;
		}
	}

	return frame_out;
}

int Filter(Mat frame_gray, int x, int y, int Oper[]){
	return  Oper[0]*frame_gray.at<unsigned char>(y-1,x-1) + Oper[1]*frame_gray.at<unsigned char>(y-1,x) + Oper[2]*frame_gray.at<unsigned char>(y-1,x+1) +
                Oper[3]*frame_gray.at<unsigned char>(y,x-1)   + Oper[4]*frame_gray.at<unsigned char>(y,x)   + Oper[5]*frame_gray.at<unsigned char>(y,x+1) +
                Oper[6]*frame_gray.at<unsigned char>(y+1,x-1) + Oper[7]*frame_gray.at<unsigned char>(y+1,x) + Oper[8]*frame_gray.at<unsigned char>(y+1,x+1);
}

Mat sikaku_kuro(Mat frame_gray, int x, int y){
	frame_gray.at<unsigned char>(y-1,x-1) = 0; frame_gray.at<unsigned char>(y-1,x) = 0; frame_gray.at<unsigned char>(y-1,x+1) = 0;
        frame_gray.at<unsigned char>(y,x-1) = 0;   frame_gray.at<unsigned char>(y,x) = 0;   frame_gray.at<unsigned char>(y,x+1) = 0;
        frame_gray.at<unsigned char>(y+1,x-1) = 0; frame_gray.at<unsigned char>(y+1,x) = 0; frame_gray.at<unsigned char>(y+1,x+1) = 0;
	return frame_gray;
}

Mat sikaku_siro(Mat frame_gray, int x, int y){
	frame_gray.at<unsigned char>(y-1,x-1) = 255; frame_gray.at<unsigned char>(y-1,x) = 255; frame_gray.at<unsigned char>(y-1,x+1) = 255;
        frame_gray.at<unsigned char>(y,x-1) = 255;   frame_gray.at<unsigned char>(y,x) = 255;   frame_gray.at<unsigned char>(y,x+1) = 255;
        frame_gray.at<unsigned char>(y+1,x-1) = 255; frame_gray.at<unsigned char>(y+1,x) = 255; frame_gray.at<unsigned char>(y+1,x+1) = 255;
	return frame_gray;
}

Mat syuusyuku(Mat frame_gray, int syori_num){
	int x,y;
        int i = 0;
        int edge[frame_gray.cols * frame_gray.rows / 2][2];
        int edge_num = 0;
        int Oper1[9] = {-1,-1,-1,-1,8,-1,-1,-1,-1};
        int p = 0;

	Mat frame_out = frame_gray;

	do{
                for(y = 0; y < frame_gray.rows; y++){
                        for(x = 0; x < frame_gray.cols; x++){
                                if(x > 10 && x < (frame_gray.cols - 11) && y > 10 && y < (frame_gray.rows - 11)){
                                        p = Filter(frame_out, x, y, Oper1);
                                        if(frame_out.at<unsigned char>(y,x) == 255 && p != 0){
                                                edge[edge_num][0] = x;
                                                edge[edge_num][1] = y;
                                                edge_num++;
                                        }
                                }else{
                                        frame_out.at<unsigned char>(y,x) = 0;
                                }
                        }
                }
                for(x = 0; x < edge_num; x++){
                        frame_out = sikaku_kuro(frame_out, edge[x][0], edge[x][1]);
                }
        edge_num = 0;
        i++;
        }while(i < syori_num);

	return frame_out;
}
Mat boutyou(Mat frame_gray, int syori_num){
	int x,y;
        int i = 0;
        int edge[frame_gray.cols * frame_gray.rows / 2][2];
        int edge_num = 0;
        int Oper1[9] = {-1,-1,-1,
						-1,8,-1,
						-1,-1,-1};
        int p = 0;

	Mat frame_out = frame_gray;

	do{
                for(y = 0; y < frame_gray.rows; y++){
                        for(x = 0; x < frame_gray.cols; x++){
                                if(x > 10 && x < (frame_gray.cols - 11) && y > 10 && y < (frame_gray.rows - 11)){
                                        p = Filter(frame_out, x, y, Oper1);
                                        if(frame_out.at<unsigned char>(y,x) == 0 && p != 0){
                                                edge[edge_num][0] = x;
                                                edge[edge_num][1] = y;
                                                edge_num++;
                                        }
                                }else{
                                        frame_out.at<unsigned char>(y,x) = 0;
                                }
                        }
                }
                for(x = 0; x < edge_num; x++){
                        frame_out = sikaku_siro(frame_out, edge[x][0], edge[x][1]);
                }
        edge_num = 0;
        i++;
        }while(i < syori_num);

	return frame_out;
}

Mat opening(Mat frame_gray, int syori_num){
	frame_gray = syuusyuku(frame_gray, syori_num);
	frame_gray = boutyou(frame_gray, syori_num);

	return frame_gray;
}
Mat closing(Mat frame_gray, int syori_num){
	frame_gray = boutyou(frame_gray, syori_num);
	frame_gray = syuusyuku(frame_gray, syori_num);

	return frame_gray;
}

Mat Filter_laplacian(Mat frame_gray, Mat frame_out){
	int x,y;
	int Oper1[9] = {-1,-1,-1,
			   -1, 8,-1,
			   -1,-1,-1};
	int p;

	//Mat frame_out = frame_gray;

	for(y = 0; y < frame_gray.rows; y++){
                for(x = 0; x < frame_gray.cols; x++){
			if(x > 0 && x < frame_gray.cols - 1 && y > 0 && y < frame_gray.rows - 1){
                        	p = (int)Filter(frame_gray, x, y, Oper1);
				if(p > 255){
					p = 255;
				}else if(p < 0){
					p = 0;
				}
			}else{
				p = 0;
			}
			frame_out.at<unsigned char>(y,x) = p;
                }
        }

	return frame_out;
}
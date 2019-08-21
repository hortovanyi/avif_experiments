#include <iostream>
#include "avif/avif.h"
#include "dav1d/dav1d.h"
#include "aom/aom.h"
#include "utils.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>

using namespace std;
using namespace cv;

int main(int, char**) {
  cout << "avif version: " << avifVersion() << endl;
  cout << "dav1d version: " << dav1d_version() << endl;
  cout << "aom version: " << aom_codec_version_str() << endl;
  cout << "OpenCV version: " << CV_VERSION << endl; 

  string avif_filename = "images/test1.avif";

  FILE * inputFile = fopen(avif_filename.c_str(), "rb");

  if (!inputFile) {
    cerr << "Can't open file for reading: " << avif_filename << endl;
    return -1;
  }

  fseek(inputFile, 0, SEEK_END);
  size_t inputFileSize = ftell(inputFile);
  fseek(inputFile, 0, SEEK_SET);

  if (inputFileSize < 1) {
    cerr << "No data in file: " << avif_filename << endl;
    fclose(inputFile);
    return -1;
  }

  avifRawData raw = AVIF_RAW_DATA_EMPTY;
  avifRawDataRealloc(&raw, inputFileSize);
  if (fread(raw.data, 1, inputFileSize, inputFile) != inputFileSize) {
    cerr << "Failed to read " << inputFileSize << " bytes from:" << avif_filename << endl;
    fclose(inputFile);
    avifRawDataFree(&raw);
    return -1;
  }

  fclose(inputFile);

  if (!avifPeekCompatibleFileType(&raw)) {
    cerr << "File is not a compatible avif or avis brand!" << endl;
    avifRawDataFree(&raw);
    return -1;
  }

  avifImage * avif_image = avifImageCreateEmpty();

  cout << "decoding avif .... " <<endl;
  avifDecoder * decoder = avifDecoderCreate();
  auto decode_start = chrono::steady_clock::now();
  avifResult decodeResult = avifDecoderRead(decoder, avif_image, &raw);
  auto decode_end = chrono::steady_clock::now();
  cout << "decoding time: " << chrono::duration_cast<chrono::milliseconds>(decode_end-decode_start).count() 
       << " ms" <<endl;
  if (decodeResult != AVIF_RESULT_OK){
    cerr << "ERROR: Failed to decode: " << avifResultToString(decodeResult) << endl;
    avifRawDataFree(&raw);
    avifDecoderDestroy(decoder);
    avifImageDestroy(avif_image);
    return -2;
  }

  auto width = avif_image->width;
  auto height = avif_image->height;
  cout << "avif image: " << avif_filename << " width: " << width
       << " height: " << height << " depth: " << avif_image->depth 
       << " format: " << avifPixelFormatToString(avif_image->yuvFormat) <<endl;

  if (avif_image->yuvFormat != AVIF_PIXEL_FORMAT_YUV420) {
    cerr << "Pixel format not supported!" <<endl;
    avifRawDataFree(&raw);
    avifDecoderDestroy(decoder);
    avifImageDestroy(avif_image);
    return -2;
  }

  cout << "converting avif from yuv to rgb ... " << endl;
  avifResult convertResult = avifImageYUVToRGB(avif_image);
  if (convertResult != AVIF_RESULT_OK) {
    cerr << "ERROR: YUV to RGB converstion failed: " << avifResultToString(convertResult) << endl;
    avifRawDataFree(&raw);
    avifDecoderDestroy(decoder);
    avifImageDestroy(avif_image);
    return -2;
  }

  cout << "creating opencv image ..." << endl;
  auto image = Mat(height, width, CV_8UC3); // 8 bit, 3 channels - bgr8

  cout << "copying planes to opencv image ..." << endl;
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      Vec3b bgr_pix(0,0,0); 
      bgr_pix[2]=avif_image->rgbPlanes[0][i + (j * avif_image->rgbRowBytes[0])]; // R
      bgr_pix[1]=avif_image->rgbPlanes[1][i + (j * avif_image->rgbRowBytes[1])]; // G
      bgr_pix[0]=avif_image->rgbPlanes[2][i + (j * avif_image->rgbRowBytes[2])]; // B
      image.at<Vec3b>(j,i)=bgr_pix;
    }
  }
  namedWindow("image", WINDOW_AUTOSIZE);
  imshow("image", image);
  waitKey(0);

  string jpg_filename = GetBaseFilename(avif_filename) + "_avif_decode.jpg";
  cout << "writing jpg_filename: " << jpg_filename << endl;
  vector<int> jpeg_params;
  jpeg_params.push_back(IMWRITE_JPEG_QUALITY);
  jpeg_params.push_back(80);
  bool result = false;
  try {
      result = imwrite(jpg_filename, image, jpeg_params);
  }
  catch (const cv::Exception& ex) {
      cerr << "Exception converting image to PNG format:" << ex.what() << endl;
  }
  if (result)
      cout << "Saved jpg file." << endl;
  else
      cerr << "ERROR: Can't save jpg file" << endl;

  avifRawDataFree(&raw);
  avifDecoderDestroy(decoder);
  avifImageDestroy(avif_image);

  return 0;
}
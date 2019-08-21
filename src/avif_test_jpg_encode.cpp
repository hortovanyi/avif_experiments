#include <iostream>
#include "avif/avif.h"
#include "aom/aom.h"
#include "utils.h"
#include "y4m.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>

using namespace std;
using namespace cv;
int main(int, char**) {
  cout << "avif version: " << avifVersion() << endl;
  cout << "aom version: " << aom_codec_version_str() << endl;
  cout << "OpenCV version: " << CV_VERSION << endl; 

  string filename = "images/test1.jpg";
  Mat image = imread(filename, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH );
  if (image.empty()) {
    cerr << "couldn't open " << filename << endl;
    return -1;
  }

  int width = image.cols;
  int height = image.rows;

  cout << "image: " << filename << " width: " << width << " heigth: " << height;
  cout << " depth: " << GetMatDepth(image) << " type: " << GetMatType(image) << endl;

  if (image.type() != CV_8UC3) {
    cerr << "unsupported type" << endl;
    return -2;
  }

  // CV_8UC3 is 8bit, 3 channels - bgr8
  int depth = 8;
  
  // namedWindow("image", WINDOW_AUTOSIZE);
  // imshow("image", image);
  // waitKey(0);

  avifPixelFormat format = AVIF_PIXEL_FORMAT_YUV420;
  avifImage * avif_image = avifImageCreate(width, height, depth, format);
  cout << "created avifImage ... " << endl;
  avifImageAllocatePlanes(avif_image, AVIF_PLANES_RGB);
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      Vec3b bgr_pix = image.at<Vec3b>(j,i);
      avif_image->rgbPlanes[0][i + (j * avif_image->rgbRowBytes[0])] = bgr_pix[2]; // R
      avif_image->rgbPlanes[1][i + (j * avif_image->rgbRowBytes[1])] = bgr_pix[1]; // G
      avif_image->rgbPlanes[2][i + (j * avif_image->rgbRowBytes[2])] = bgr_pix[0]; // B
    }
  }
  cout << "allocated plains ... " << endl;

  // avifNclxColorProfile nclx;
  // nclx.colourPrimaries = AVIF_NCLX_COLOUR_PRIMARIES_BT709;
  // nclx.transferCharacteristics = AVIF_NCLX_TRANSFER_CHARACTERISTICS_GAMMA22;
  // nclx.matrixCoefficients = AVIF_NCLX_MATRIX_COEFFICIENTS_BT709;
  // nclx.fullRangeFlag = AVIF_NCLX_FULL_RANGE;
  // avifImageSetProfileNCLX(avif_image, &nclx);


  avifRawData output = AVIF_RAW_DATA_EMPTY;
  avifEncoder * encoder = avifEncoderCreate();
  encoder->maxThreads = 4; // Choose max encoder threads, 1 to disable multithreading
  // encoder->minQuantizer = AVIF_QUANTIZER_LOSSLESS;
  // encoder->maxQuantizer = AVIF_QUANTIZER_LOSSLESS;
  encoder->minQuantizer = AVIF_QUANTIZER_LOSSLESS;
  encoder->maxQuantizer = AVIF_QUANTIZER_WORST_QUALITY;
  // Tiling values range [0-6], where the value indicates a request for 2^n tiles in that dimension.
  encoder->tileRowsLog2 = 2;
  encoder->tileColsLog2 = 2;

  cout << "encoding avif ... " << endl;
  auto encode_start = chrono::steady_clock::now();
  avifResult encodeResult = avifEncoderWrite(encoder, avif_image, &output);
  auto encode_end = chrono::steady_clock::now();
  cout << "encoding time:" << chrono::duration_cast<chrono::milliseconds>(encode_end-encode_start).count() 
       << " ms" <<endl;
  if (encodeResult == AVIF_RESULT_OK) {
      cout << "* ColorOBU size: " << encoder->ioStats.colorOBUSize << " bytes" << endl;
      cout << "output.size: " << output.size << " bytes" << endl;

      string avif_filename = GetBaseFilename(filename) + ".avif";
      cout << "writing avif_filename: " << avif_filename << endl;

      // output contains a valid .avif file's contents
      // output.data;
      // output.size;
      FILE * f = fopen(avif_filename.c_str(), "wb");
      if (f) {
        fwrite(output.data, 1, output.size, f);
        fclose(f);
      } else {
        cerr << "failed to open avif file for write ...";
        return -3;
      }

      string y4m_filename = GetBaseFilename(filename) + ".y4m";
      cout << "writing y4m_filename: " << y4m_filename << endl;
      y4mWrite(avif_image, y4m_filename.c_str());
  } else {
      printf("ERROR: Failed to encode: %s\n", avifResultToString(encodeResult));
  }
  avifEncoderDestroy(encoder);
  avifImageDestroy(avif_image);
  avifRawDataFree(&output);

}

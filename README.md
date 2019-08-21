# AVIF Experiments

AVIF (AV1 Still Image File Format) is a new still image format based on AV1. 

In this project, a [test jpg image](images/test1.jpg) is encoded into an avif file and stored in the [images](images/) directory. At present the starting image names are hardcoded, and subsequent images are written with appended names (minus the . type).  

It has two executable programs:
1. avif_test_jpg_encode - takes the [test1.jpg](images/test1.jpg) image and outputs [test1.avif](images/test1.avif) and [test1.y4m](images/test1.y4m)
2. avif_decode_test - uses [test1.avif](images/test.avif) and displays the images as well as writes it to [test1_avif_decode.jpg]

## Dependencies
The build has dependencies on [OpenCV](https://opencv.org), [libaom](https://aomedia.googlesource.com/aom/), [libavif](https://github.com/AOMediaCodec/libavif) and [dav1d](https://github.com/videolan/dav1d). As libavif had no install instructions, the libavif.a was copied to a project lib directory, with the avif/avif.h copied the include folder.

## Building
```
mkdir build
cd build
cmake ..
make
```

When succesful, there will be two executables per above.

## Results

As can be seen by the following encoding log output, the time for encoding was 8.5 seconds (originally around 12 seconds). I tried experimenting with the number of threads and tiling settings, but it only had a slight improvement. Searching around on the internet, it suggested that libaom, encoding is indeed slow. 

Changing the Quantizer values from AVIF_QUANTIZER_LOSSLESS, dramaticully decreased the size of the test1.avif file to ~30 KB. This is a significant decrease from the original test1.jpg file size of ~282 KB.

The decoding time was 45 milliseconds for test1.avif in the Decoding log. Which was very impressive. Note I'd compiled libavf to decode using dav1d.

At present, based on the encoding time, AVIF isn't suitable, for the intended need (of a different project) for capturing and writing images to disk at 30 fps(frames per second) becuase of the slow encoding time per frame.

### Encoding Log

```
(base)  nick@xps  ~/dev/test/avif_experiments   master  build/avif_test_jpg_encode
avif version: 0.3.6
aom version: 1.0.0-2236-g0233da474
OpenCV version: 4.1.1-dev
image: images/test1.jpg width: 1280 heigth: 720 depth: CV_8U type: CV_8UC3
created avifImage ... 
allocated plains ... 
encoding avif ... 
encoding time:8568 ms
* ColorOBU size: 30384 bytes
output.size: 30646 bytes
writing avif_filename: images/test1.avif
writing y4m_filename: images/test1.y4m
Wrote: images/test1.y4m
```

### Decoding Log

```
(base)  nick@xps  ~/dev/test/avif_experiments   master  build/avif_decode_test                                                  
avif version: 0.3.6
dav1d version: 0.4.0-19-g3d94fb9
aom version: 1.0.0-2236-g0233da474
OpenCV version: 4.1.1-dev
decoding avif .... 
decoding time: 45 ms
avif image: images/test1.avif width: 1280 height: 720 depth: 8 format: YUV420
converting avif from yuv to rgb ... 
creating opencv image ...
copying planes to opencv image ...
writing jpg_filename: images/test1_avif_decode.jpg
Saved jpg file.
```

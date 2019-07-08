#include <jni.h>
#include <string>
#include <android/log.h>
#include <time.h>
#include <stdio.h>

#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/dnn.h>
#include <dlib/clustering.h>




#define  LOG_TAG    "someTag"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)


using namespace dlib;
using namespace std;


dlib::frontal_face_detector detector = dlib::frontal_face_detector();

std::vector<int > getfacerect(const std::vector<int>img,int height,int width)
{

    dlib::array2d<unsigned char>image;
    image.set_size(height,width);


    for (int i = 0; i < height; i++)
    {
        for(int j=0;j<width;j++)
        {
            int clr = img[i*width+j];
            int red = (clr & 0x00ff0000) >> 16; // 取高两位
            int green = (clr & 0x0000ff00) >> 8; // 取中两位
            int blue = clr & 0x000000ff; // 取低两位
            unsigned char gray=red*0.299+green*0.587+blue*0.114;
            //dlib::rgb_pixel pt(red,green,blue);
            image[i][j]=gray;
        }
    }

    std::vector<dlib::rectangle> dets= detector(image);


    std::vector<int>result;

    if (!dets.empty())
    {
        result.push_back(dets.size());

        for(int i = 0;i < dets.size(); i ++){
            result.push_back(dets[i].left());
            result.push_back(dets[i].top());
            result.push_back(dets[i].right());
            result.push_back(dets[i].bottom());
        }
    }
    return result;

}

extern "C"
JNIEXPORT void JNICALL
Java_du_com_myapplication_DlibNative_readDataDectection(JNIEnv *env, jclass type) {
    detector = dlib::get_frontal_face_detector();
}

extern "C" JNIEXPORT jintArray JNICALL Java_du_com_myapplication_DlibNative_detecFace
        (JNIEnv *env, jclass clazz, jintArray image_data, jint image_width, jint image_height) {



    LOGD("this time start 1 %f", (float)clock()/CLOCKS_PER_SEC);
    std::vector<int>image_datacpp(image_height*image_width);
    jsize len = env->GetArrayLength(image_data);
    jint *body = env->GetIntArrayElements(image_data, 0);
    for (jsize i=0;i<len;i++){
        image_datacpp[i]=(int)body[i];
    }

    std::vector<int>rect = getfacerect(image_datacpp,image_height,image_width);

    int size_int=rect.size();

    jintArray result = env->NewIntArray(size_int);
    env->SetIntArrayRegion(result, 0, size_int,&rect[0]);
    LOGD("this time start 2 %f", (float)clock()/CLOCKS_PER_SEC);
    return result;
}

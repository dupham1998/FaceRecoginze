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


dlib::frontal_face_detector detector;

std::vector<int > getfacerect(const std::vector<int>img,int height,int width)
{

    dlib::array2d<unsigned char>image;
    image.set_size(height,width);


    for (int i = 0; i < height; i++)
    {
        for(int j=0;j<width;j++)
        {
            int clr = img[i*width+j];
            int red = (clr & 0x00ff0000) >> 16;
            int green = (clr & 0x0000ff00) >> 8;
            int blue = clr & 0x000000ff;
            unsigned char gray=red*0.299+green*0.587+blue*0.114;

            image[i][j]=gray;
        }
    }





    // a LOGD("this time start 3 %f", (float)clock()/CLOCKS_PER_SEC);
    std::vector<dlib::rectangle> dets= detector(image);
    //  LOGD("this time start 4 %f", (float)clock()/CLOCKS_PER_SEC);

    //   LOGD("this time  end is  %f", (float)clock()/CLOCKS_PER_SEC);
    std::vector<int>rect;

    rect.push_back(dets.size());

    for(int i = 0; i < dets.size(); i ++){
        rect.push_back(dets[i].left());
        rect.push_back(dets[i].top());
        rect.push_back(dets[i].right());
        rect.push_back(dets[i].bottom());


        LOGD("this is 1 %d %d %d %d", dets[i].left(), dets[i].top(), dets[i].right(), dets[i].bottom());
    }
    return rect;

}

extern "C"
JNIEXPORT void JNICALL
Java_du_com_myapplication_DlibNative_readDataDectection(JNIEnv *env, jclass type) {
    detector = dlib::get_frontal_face_detector();
}

extern "C" JNIEXPORT jintArray JNICALL Java_du_com_myapplication_DlibNative_detecFace
        (JNIEnv *env, jclass clazz, jintArray image_data, jint image_width, jint image_height) {



    LOGD("this time start 1 %f", (float)clock()/CLOCKS_PER_SEC);
    std::vector<int> image_datacpp(image_height * image_width);
    jsize len = env->GetArrayLength(image_data);
    jint *body = env->GetIntArrayElements(image_data, 0);

    for (jsize i=0;i<len;i++){
        image_datacpp[i]=(int)body[i];
    }


    std::vector<int>rect=getfacerect(image_datacpp,image_height,image_width);


    jintArray result =env->NewIntArray(rect.size());
    left   
	
    LOGD("this time start 2 %f", (float)clock()/CLOCKS_PER_SEC);
    return result;
}

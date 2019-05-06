//
// Created by MyPC on 25/03/2019.
//
#include <jni.h>
#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/svm_threaded.h>
#include <dlib/rand.h>
#include <android/log.h>
#include <string>
#include <time.h>
#include <fstream>

#define  LOG_TAG    "someTag"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace std;
using namespace dlib;


template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N, BN, 1, tag1<SUBNET>>>;

template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2, 2, 2, 2, skip1<tag2<block<N, BN, 2, tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block = BN<con<N, 3, 3, 1, 1, relu<BN<con<N, 3, 3, stride, stride, SUBNET>>>>>;

template <int N, typename SUBNET> using ares = relu<residual<block, N, affine, SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block, N, affine, SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
template <typename SUBNET> using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;

//----------------bien toan cuc-----------------------------------------//
using anet_type = loss_metric<fc_no_bias<128, avg_pool_everything<
        alevel0<
                alevel1<
                        alevel2<
                                alevel3<
                                        alevel4<
                                                max_pool<3, 3, 2, 2, relu<affine<con<32, 7, 7, 2, 2,
                                                        input_rgb_image_sized<150>
                                                >>>>>>>>>>>>;
typedef one_vs_one_trainer<any_trainer<matrix<double, 128, 1> > > ovo_trainer;
typedef polynomial_kernel<matrix<double, 128, 1>> poly_kernel;
typedef radial_basis_kernel<matrix<double, 128, 1>> rbf_kernel;
one_vs_one_decision_function<ovo_trainer,
        decision_function<poly_kernel>,  // This is the output of the poly_trainer
        decision_function<rbf_kernel>    // This is the output of the rbf_trainer
> df2; // SVM

std::vector <string> names; // ten cua cac khuon mat trong data
std::vector <matrix<double, 128, 1> > med_face; // gia tri trung binh 128d cua cac khuon mat
shape_predictor sp;
anet_type net;
dlib::frontal_face_detector face_detector;
//--------------------------------------------------------------------------------------------//


// tao data chuyen tu maxtrix<float, 0, 1> sang matrix<double, 128,  1>
void generate_data(
        std::vector<matrix<double, 128, 1> >& samples,
        std::vector<matrix<float, 0, 1>> data_face_descriptors)
{
    matrix<double, 128, 1> tmp;

    for (int i = 0; i < data_face_descriptors.size(); i++) {
        for (int j = 0; j < 128; j++) {
            tmp(j) = (double)data_face_descriptors[i](j);
        }
        samples.push_back(tmp);
    }
}

// doc ten vao vector<string> names va doc cac gia tri trung binh cua tung khuon mae
void readData(){
    // doc ten vao name
    ifstream fi2("/storage/emulated/0/dlib_rec_example/name.txt");
    int N; fi2 >> N;
    for (int i = 0; i < N; i++) {
        string ten;
        fi2 >> ten;
        names.push_back(ten);
    }
    fi2.close();

    // doc gia tri trung binh
    ifstream fi("/storage/emulated/0/dlib_rec_example/med_face.txt");
    int n; fi >> n;
    matrix<double, 128, 1> tmp;
    tmp(0) = 1; med_face.push_back(tmp);

    for (int i = 1; i <= n; i++) {
        matrix<double, 128, 1> a;
        double t;
        for (int j = 0; j < 128; j++) {
            fi >> t;
            a(j) = t;
        }
        med_face.push_back(a);
    }
    fi.close();

    face_detector = dlib::get_frontal_face_detector();
    // load model svm
    deserialize("/storage/emulated/0/dlib_rec_example/df_test.dat") >> df2;
    // load model resnet
    deserialize("/storage/emulated/0/dlib_rec_example/dlib_face_recognition_resnet_model_v1.dat") >> net;
    // load model shape 5 predictor
    deserialize("/storage/emulated/0/dlib_rec_example/shape_predictor_5_face_landmarks.dat") >> sp;
}
// gan_nhan cho tung khuon mat
std::vector <int> gan_nhan(std:: vector<matrix<double, 128, 1>> faces){

    std::vector <int> result;

    for (int i = 0; i < faces.size(); i++) {
        int cs = (int)df2(faces[i]);
        if (length(faces[i] - med_face[cs]) >= 0.52) cs = 0;
        result.push_back(cs);
    }

    return result;
}

// nhan dien khuon mat
void nhan_dien(const std::vector<int>img,int height,int width, std::vector<string>& labels, std::vector<int> rect){

    array2d<unsigned char> image;
    image.set_size(height,width);

    // tao lai anh
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

    LOGD("this time 1.5 %f", (float)clock()/CLOCKS_PER_SEC);


    std::vector<dlib::rectangle> dets;
    std::vector<matrix<rgb_pixel>> faces; // face chip
    dets  = face_detector(image);

    LOGD("this time 2 %f", (float)clock()/CLOCKS_PER_SEC);

    // tinh moc tren tung khuon mat
    for(auto face : dets){
        LOGD("this is 3 %d %d %d %d", face.left(), face.top(), face.right(), face.bottom());
        auto  shape = sp(image, face);
        matrix<rgb_pixel> face_chip;
        dlib::extract_image_chip(image, get_face_chip_details(shape, 150, 0.25), face_chip);
        faces.push_back(move(face_chip));
    }

    LOGD("this time  3 %f", (float)clock()/CLOCKS_PER_SEC);

    if(faces.size() != 0){

        std::vector <int> result;

        // tinh 128D cho moi khuon mat
        std::vector<matrix<float, 0, 1>> face_descriptors = net(faces);
        LOGD("co %d\n",face_descriptors.size());
        LOGD("this time 4 %f", (float)clock()/CLOCKS_PER_SEC);

        std::vector<matrix<double , 128, 1>> face;
        generate_data(face, face_descriptors);

        // gan nhan
        result = gan_nhan(face);
        LOGD("this time 5 %f", (float)clock()/CLOCKS_PER_SEC);


        for(int i = 0; i < result.size(); i ++){
            labels.push_back(names[result[i]]);
        }
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_du_com_myapplication_DlibNative_faceRecoginze(JNIEnv *env, jclass type, jintArray image_data,
                                                   jint image_width, jint image_height) {
    std::vector<int> image_datacpp(image_height * image_width);
    jsize len = env->GetArrayLength(image_data);
    jint *body = env->GetIntArrayElements(image_data, 0);

    for (jsize i=0;i<len;i++){
        image_datacpp[i]=(int)body[i];
    }

    readData();
 //   nhan_dien(image_datacpp, image_height, image_width);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_du_com_myapplication_DlibNative_faceRecogintion(
        JNIEnv *env, jclass type, jintArray image_data, jint image_width, jint image_height, jintArray rect) {

 //   readData();

    // lay thong tin anh
    std::vector<int> image_datacpp(image_height *image_width);
    jsize len = env->GetArrayLength(image_data);
    jint *body = env->GetIntArrayElements(image_data, 0);

    for (jsize i = 0; i < len; i++) {
        image_datacpp[i] = (int) body[i];
    }

    // lay thong tin ve vi tri khuon mat
    std::vector<int> rect_face;
    jsize length = env->GetArrayLength(rect);
    jint* data   = env->GetIntArrayElements(rect, 0);

    for(jsize i = 0; i < length; i ++){
        rect_face.push_back((int)data[i]);
    }


    // nhan tra lai
    std::vector<string> labels;
    nhan_dien(image_datacpp, image_height, image_width, labels, rect_face);

    jobjectArray  ret;
    ret = (jobjectArray)env->NewObjectArray(labels.size(),
                                           env->FindClass("java/lang/String"),
                                           env->NewStringUTF(""));
    for(int i=0; i < labels.size(); i++) {
        env->SetObjectArrayElement(
                ret,i,env->NewStringUTF(labels[i].c_str()));
    }

    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_du_com_myapplication_DlibNative_readDataFaceRecogintion(JNIEnv *env, jclass type) {
    readData();
}

package du.com.myapplication;

import android.os.Environment;

import java.io.File;

public class MakeFolder {
    public static String getDLibDirectoryPath() {
        File sdcard = Environment.getExternalStorageDirectory();
        String targetPath = sdcard.getAbsolutePath() + File.separator + "dlib_rec_example";
        return targetPath;
    }

    public static String getDLibImageDirectoryPath() {
        String targetPath = getDLibDirectoryPath()+ File.separator + "images";
        return targetPath;
    }

    public static String getFaceShapeModelPath() {
        String targetPath = getDLibDirectoryPath() + File.separator + "shape_predictor_5_face_landmarks.dat";
        return targetPath;
    }

    public static String getFaceDescriptorModelPath() {
        String targetPath = getDLibDirectoryPath() + File.separator + "dlib_face_recognition_resnet_model_v1.dat";
        return targetPath;
    }
    public static String getSVMmodelPath(){
        String targetPath = getDLibDirectoryPath() + File.separator + "df_test.dat";
        return targetPath;
    }
    public static String getNamePath() {
        String targetPath = getDLibDirectoryPath() + File.separator + "name.txt";
        return targetPath;
    }
    public static String getMedFacePath() {
        String targetPath = getDLibDirectoryPath() + File.separator + "med_face.txt";
        return targetPath;
    }
}



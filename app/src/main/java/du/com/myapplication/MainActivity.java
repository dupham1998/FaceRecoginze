package du.com.myapplication;


import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.util.Printer;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.VideoView;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.JavaCameraView;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;


import java.io.File;
import java.io.IOException;


public class MainActivity extends AppCompatActivity {


    private static final int CAMERA_PERMISSION = 0;
    private static final int READ_PERMISSION = 1;
    private static final int WRITE_PERMISSION = 2;
    Button faceDectect, faceRecognize;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);



        copyResToStorge();
        // tao cac nut cho main activity
        faceDectect = (Button) findViewById(R.id.detectFace);
        faceRecognize = (Button) findViewById(R.id.recognize);
        faceDectect.setOnClickListener(onClick);
        faceRecognize.setOnClickListener(onClick);

        // read data cho dlib
        DlibNative.readDataDectection();
        DlibNative.readDataFaceRecogintion();
    }

    public void openDetectAcitivity(){
        Intent intent = new Intent(this, DetectActivity.class);
        startActivity(intent);
    }

    public void openRecognizeActivity(){
        Intent intent = new Intent(this, RecognizeActivity.class);
        startActivity(intent);
    }

    public View.OnClickListener onClick = new View.OnClickListener(){

        @Override
        public void onClick(View v) {
            if(v.equals(faceDectect)){
                openDetectAcitivity();
            }
            if(v.equals(faceRecognize)){
                openRecognizeActivity();
            }
        }
    };

    private void copyResToStorge(){
        File folder = new File(MakeFolder.getDLibDirectoryPath());
        boolean success = false;
        if (!folder.exists()) {
            success = folder.mkdirs();
        }
        if (success) {
            File image_folder = new File(MakeFolder.getDLibImageDirectoryPath());
            image_folder.mkdirs();

            if (!new File(MakeFolder.getFaceShapeModelPath()).exists()) {
                FileUtils.copyFileFromRawToOthers(MainActivity.this, R.raw.shape_predictor_5_face_landmarks, MakeFolder.getFaceShapeModelPath());
            }
            if (!new File(MakeFolder.getFaceDescriptorModelPath()).exists()) {
                FileUtils.copyFileFromRawToOthers(MainActivity.this, R.raw.dlib_face_recognition_resnet_model_v1, MakeFolder.getFaceDescriptorModelPath());
            }
            if (!new File(MakeFolder.getSVMmodelPath()).exists()) {
                FileUtils.copyFileFromRawToOthers(MainActivity.this, R.raw.df_test, MakeFolder.getSVMmodelPath());
            }
            if (!new File(MakeFolder.getNamePath()).exists()) {
                FileUtils.copyFileFromRawToOthers(MainActivity.this, R.raw.name, MakeFolder.getNamePath());
            }
            if (!new File(MakeFolder.getMedFacePath()).exists()) {
                FileUtils.copyFileFromRawToOthers(MainActivity.this, R.raw.med_face, MakeFolder.getMedFacePath());
            }
        } else {
           // Log.d("test", "error in setting dlib_rec_example directory");
        }
    }

    private void requestPermission(){
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M){
            if(checkSelfPermission(Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED){
                requestPermissions(new String[]{Manifest.permission.CAMERA}, CAMERA_PERMISSION);
            }
            if(checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED){
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, READ_PERMISSION);
            }
            if(checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED){
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, WRITE_PERMISSION);
            }
        }
    }
}

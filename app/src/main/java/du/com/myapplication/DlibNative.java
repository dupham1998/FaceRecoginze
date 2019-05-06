package du.com.myapplication;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;

public class DlibNative {
    private static final float FACE_DOWNSAMPLE_RATIO = 4;
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("openblas");
    }

    public static void faceDetect(Bitmap origin_image){
        // chuyen anh thanh mang 1 chieu
        float scale = 240.f/ Math.max(origin_image.getHeight(), origin_image.getWidth());
        int width = (int)(origin_image.getWidth()*scale);
        int height = (int)(origin_image.getHeight()*scale);
        Bitmap resize_image=Bitmap.createScaledBitmap(origin_image,width,height , false);

        int[] pixels = new int[width * height];
        resize_image.getPixels(pixels, 0, width, 0, 0, width, height);

        // detect khuon mat tron anh
        int[] rect = detecFace(pixels, width, height);

        // chinh lai toa do theo scale
        for(int i = 1; i < rect.length ; i ++)   rect[i] = (int) (rect[i] / scale);

        // ve hinh vuong bao quanh mat
        Canvas canvas = new Canvas(origin_image);
        Paint p = new Paint();
        p.setColor(Color.BLUE);
        p.setStrokeWidth(10.0f);

        for(int i = 1; i <= rect[0]; i ++){
            int index = (i - 1) * 4 + 1;
            canvas.drawLine(rect[index + 0], rect[index + 1], rect[index + 2], rect[index + 1], p);
            canvas.drawLine(rect[index + 0], rect[index + 1], rect[index + 0], rect[index + 3], p);
            canvas.drawLine(rect[index + 0], rect[index + 3], rect[index + 2], rect[index + 3], p);
            canvas.drawLine(rect[index + 2], rect[index + 1], rect[index + 2], rect[index + 3], p);
        }
    }

    public static void faceRecoginze(Bitmap origin_image){
        // chuyen anh thanh mang
        float scale = 1 / FACE_DOWNSAMPLE_RATIO;
        int width = (int)(origin_image.getWidth()*scale);
        int height = (int)(origin_image.getHeight()*scale);

        Log.d("test", ": " + width + height);
        Bitmap resize_image=Bitmap.createScaledBitmap(origin_image,width,height , false);

        int[] pixels = new int[width * height];
        resize_image.getPixels(pixels, 0, width, 0, 0, width, height);

        // detect khuon mat va gan nhan
        int[] rect = detecFace(pixels, width, height);
        String[] label = faceRecogintion(pixels, width, height, rect);


        // chinh lai toa do theo scale
        for(int i = 1; i < rect.length; i ++) rect[i] = (int) (rect[i] / scale);

        // ve hinh vuong va gan ten
        Canvas canvas = new Canvas(origin_image);
        Paint p = new Paint();
        p.setColor(Color.BLUE);
        p.setStrokeWidth(10.0f);

        Paint paint = new Paint();
        paint.setStyle(Paint.Style.FILL);
        paint.setColor(Color.WHITE); // Text Color
        paint.setTextSize(80); //Text Size

        for(int i = 1; i <= rect[0]; i ++){
            int index = (i - 1) * 4 + 1;
            canvas.drawLine(rect[index + 0], rect[index + 1], rect[index + 2], rect[index + 1], p);
            canvas.drawLine(rect[index + 0], rect[index + 1], rect[index + 0], rect[index + 3], p);
            canvas.drawLine(rect[index + 0], rect[index + 3], rect[index + 2], rect[index + 3], p);
            canvas.drawLine(rect[index + 2], rect[index + 1], rect[index + 2], rect[index + 3], p);

            canvas.drawText(label[i - 1], rect[index + 0], rect[index + 3], paint);
        }
    }

    private native static int[] detecFace(int[] data, int witdh, int height);
    private native static void faceRecoginze(int data[], int witdh, int height);
    private native static String[] faceRecogintion(int data[], int width, int height, int[] rect);
    public native static void readDataFaceRecogintion();
    public native static void readDataDectection();
}

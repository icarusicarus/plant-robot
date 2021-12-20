package com.example.myapplication_3;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import android.media.Image;
import android.os.Bundle;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.view.View;
import android.view.accessibility.AccessibilityManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    ImageView bluetoothStatus;
    ImageView direction;
    Button mBtnBluetoothOn;
    Button mBtnBluetoothOff;
    ImageButton mBtnConnect;
    Button mBtnSendData;
    ImageButton btnUp;
    ImageButton btnDown;
    ImageButton btnLeft;
    ImageButton btnRight;
    ImageButton btnStop;
    ImageButton btnSun;

    BluetoothAdapter mBluetoothAdapter;
    Set<BluetoothDevice> mPairedDevices;
    List<String> mListPairedDevices;

    Handler mBluetoothHandler;
    ConnectedBluetoothThread mThreadConnectedBluetooth;
    BluetoothDevice mBluetoothDevice;
    BluetoothSocket mBluetoothSocket;

    final static int BT_REQUEST_ENABLE = 1;
    final static int BT_MESSAGE_READ = 2;
    final static int BT_CONNECTING_STATUS = 3;
    final static UUID BT_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); // 블루투스 UUID 생성

    private long backBtnTime = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ActionBar actionBar = getSupportActionBar();
        actionBar.hide();

        bluetoothStatus = (ImageView)findViewById(R.id.bluetoothstatus);
        mBtnBluetoothOn = (Button)findViewById(R.id.btnBluetoothOn);
        mBtnBluetoothOff = (Button)findViewById(R.id.btnBluetoothOff);
        mBtnConnect = (ImageButton)findViewById(R.id.btnConnect);


        btnUp=(ImageButton)findViewById(R.id.btnUp);
        btnDown=(ImageButton)findViewById(R.id.btnDown);
        btnRight=(ImageButton)findViewById(R.id.btnRight);
        btnLeft = (ImageButton)findViewById(R.id.btnLeft);
        btnStop = (ImageButton)findViewById(R.id.btnStop);
        btnSun = (ImageButton)findViewById(R.id.btnSun);

        TextView temp = (TextView)findViewById(R.id.temp);
        TextView humd = (TextView)findViewById(R.id.humd);
        TextView ilm = (TextView)findViewById(R.id.ilm);


        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        mBtnBluetoothOn.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                bluetoothOn();
                bluetoothStatus.setImageDrawable(getResources().getDrawable(R.drawable.bluetooth_on));
            }
        });
        mBtnBluetoothOff.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                bluetoothOff();
                bluetoothStatus.setImageDrawable(getResources().getDrawable(R.drawable.bluetooth_off));
            }
        });
        mBtnConnect.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                listPairedDevices();
            }
        });


// 버튼을 길게 누르고 있으면 1초마다 데이터를 전송하는 리스너

        // up 버튼 눌렀을 때
        btnUp.setOnTouchListener(new LongPressRepeatListener(500, 1000, new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mThreadConnectedBluetooth != null){
                    mThreadConnectedBluetooth.write("1"); // 데이터 전송
                    Toast.makeText(MainActivity.this, "up",Toast.LENGTH_SHORT).show();
                }
            }

        }));

        // down 버튼 눌렀을 때
        btnDown.setOnTouchListener(new LongPressRepeatListener(500, 1000, new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mThreadConnectedBluetooth != null){
                    mThreadConnectedBluetooth.write("2");
                    Toast.makeText(MainActivity.this, "down",Toast.LENGTH_SHORT).show();
                }
            }

        }));

        // left 버튼 눌렀을 때
        btnLeft.setOnTouchListener(new LongPressRepeatListener(500, 1000, new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mThreadConnectedBluetooth != null){
                    mThreadConnectedBluetooth.write("3");
                    Toast.makeText(MainActivity.this, "left",Toast.LENGTH_SHORT).show();
                }
            }

        }));

        // right 버튼 눌렀을 때
        btnRight.setOnTouchListener(new LongPressRepeatListener(500, 1000, new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mThreadConnectedBluetooth != null){
                    mThreadConnectedBluetooth.write("4");
                    Toast.makeText(MainActivity.this, "right",Toast.LENGTH_SHORT).show();
                }
            }

        }));

        // stop 버튼 눌렀을 때
        btnStop.setOnTouchListener(new LongPressRepeatListener(500, 1000, new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mThreadConnectedBluetooth != null){
                    mThreadConnectedBluetooth.write("5");
                    Toast.makeText(MainActivity.this, "stop",Toast.LENGTH_SHORT).show();
                }
            }

        }));


        // 햇빛찾기 버튼 눌렀을 때
        btnSun.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mThreadConnectedBluetooth != null){
                    mThreadConnectedBluetooth.write("6");
                }
            }
        });
        // 데이터 수신을 위한 handler
        mBluetoothHandler = new Handler(){
            public void handleMessage(android.os.Message msg){
                if(msg.what == BT_MESSAGE_READ){
                    String readMessage = null;
                    try {
                        // 인코딩된 바이트들을 문자열로 전환
                        readMessage = new String((byte[]) msg.obj, "UTF-8");
                    } catch (UnsupportedEncodingException e) {
                        e.printStackTrace();
                    }
                    String[] array = readMessage.split(",");
                    if(array[2]=="0"){
                        if(mBluetoothHandler != null){
                            mBluetoothHandler.removeCallbacksAndMessages(null);
                        }
                    }

                    temp.setText(array[0].concat("C"));
                    humd.setText(array[1].concat("%"));
                    ilm.setText(array[2].concat("lx"));
                }
            }
        };
    }
    // 블루투스 켜기
    void bluetoothOn() {
        if(mBluetoothAdapter == null) {
            Toast.makeText(getApplicationContext(), "블루투스를 지원하지 않는 기기입니다.", Toast.LENGTH_LONG).show();
        }
        else {
            if (mBluetoothAdapter.isEnabled()) { // 블루투스가 활성화되어있다면
                Toast.makeText(getApplicationContext(), "블루투스가 이미 활성화 되어 있습니다.", Toast.LENGTH_LONG).show();
            }
            else { // 블루투스가 활성화되어있지 않다면
                Toast.makeText(getApplicationContext(), "블루투스가 활성화 되어 있지 않습니다.", Toast.LENGTH_LONG).show();
                Intent intentBluetoothEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(intentBluetoothEnable, BT_REQUEST_ENABLE);
            }
        }
    }
    // 블루투스 끄기
    void bluetoothOff() {
        if (mBluetoothAdapter.isEnabled()) { // 블루투스가 켜져 있다면
            mBluetoothAdapter.disable();
            Toast.makeText(getApplicationContext(), "블루투스 비활성화", Toast.LENGTH_SHORT).show();
        }
        else {
            Toast.makeText(getApplicationContext(), "블루투스가 이미 비활성화 되어 있습니다.", Toast.LENGTH_SHORT).show();
        }
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case BT_REQUEST_ENABLE:
                if (resultCode == RESULT_OK) { // 블루투스 활성화를 확인을 클릭하였다면
                    Toast.makeText(getApplicationContext(), "블루투스 활성화", Toast.LENGTH_SHORT).show();

                } else if (resultCode == RESULT_CANCELED) { // 블루투스 활성화를 취소를 클릭하였다면
                    Toast.makeText(getApplicationContext(), "취소", Toast.LENGTH_LONG).show();
                    bluetoothStatus.setImageDrawable(getResources().getDrawable(R.drawable.bluetooth_off));
                }
                break;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }
    void listPairedDevices() {
        if (mBluetoothAdapter.isEnabled()) {
            //페어링되어있는 기기 검색
            mPairedDevices = mBluetoothAdapter.getBondedDevices();
            // 연결할 디바이스를 선택하는 대화상자 띄우기
            if (mPairedDevices.size() > 0) { // 페어링된 디바이스가 있으면
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setTitle("장치 선택");

                mListPairedDevices = new ArrayList<String>();
                // 페어링된 모든 디바이스를 리스트에 추가
                for (BluetoothDevice device : mPairedDevices) {
                    mListPairedDevices.add(device.getName());
                    //mListPairedDevices.add(device.getName() + "\n" + device.getAddress());
                }
                final CharSequence[] items = mListPairedDevices.toArray(new CharSequence[mListPairedDevices.size()]);
                mListPairedDevices.toArray(new CharSequence[mListPairedDevices.size()]);

                // 디바이스를 선택했을 때
                builder.setItems(items, new DialogInterface.OnClickListener() {
                    @Override
                    //디바이스와 연결하는 함수 호출
                    public void onClick(DialogInterface dialog, int item) {
                        connectSelectedDevice(items[item].toString());
                    }
                });
                //대화창 띄우기
                AlertDialog alert = builder.create();
                alert.show();
            } else { // 페어링된 디바이스가 없으면
                Toast.makeText(getApplicationContext(), "페어링된 장치가 없습니다.", Toast.LENGTH_LONG).show();
            }
        }
        else {
            Toast.makeText(getApplicationContext(), "블루투스가 비활성화 되어 있습니다.", Toast.LENGTH_SHORT).show();
        }
    }
    void connectSelectedDevice(String selectedDeviceName) {
        for(BluetoothDevice tempDevice : mPairedDevices) { // 페어링된 디바이스 모두 탐색
            if (selectedDeviceName.equals(tempDevice.getName())) { // 사용자가 설정한 디바이스와 같은 이름이면
                mBluetoothDevice = tempDevice; // 블루투스 디바이스로 설정
                break;
            }
        }
        try { // 디바이스와 연결하는 소켓 생성
            mBluetoothSocket = mBluetoothDevice.createRfcommSocketToServiceRecord(BT_UUID);
            mBluetoothSocket.connect(); // 연결 시작
            mThreadConnectedBluetooth = new ConnectedBluetoothThread(mBluetoothSocket);
            mThreadConnectedBluetooth.start();
            Toast.makeText(MainActivity.this, "connect",Toast.LENGTH_SHORT).show();
            mBluetoothHandler.obtainMessage(BT_CONNECTING_STATUS, 1, -1).sendToTarget();
        } catch (IOException e) { // 연결 중 오류가 발생했을 시
            Toast.makeText(getApplicationContext(), "블루투스 연결 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
        }
    }
    private class ConnectedBluetoothThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public ConnectedBluetoothThread(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try {
                // 통신을 위한 inputstream, outputstream 가져오기
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "소켓 연결 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }
        public void run() {
            byte[] buffer = new byte[1024];
            int bytes;

            while (true) {
                try {

                    bytes = mmInStream.available();
                    if (bytes != 0) { // 송신된 데이터가 존재한다면
                        SystemClock.sleep(1000); // 1초마다 받아오기
                        bytes = mmInStream.available();
                        bytes = mmInStream.read(buffer, 0, bytes); // 데이터 읽어오기
                        mBluetoothHandler.obtainMessage(BT_MESSAGE_READ, bytes, -1, buffer).sendToTarget();
                    }
                } catch (IOException e) {
                    break;
                }
            }
        }
        public void write(String str) { // 디바이스에 데이터 전송
            byte[] bytes = str.getBytes();
            try {
                mmOutStream.write(bytes);
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "데이터 전송 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }
        }
        public void cancel() { // 블루투스 소켓 닫기
            try {
                mmSocket.close();
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "소켓 해제 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }
        }
    }

    public void onBackPressed(){
        long curTime = System.currentTimeMillis();
        long gapTime = curTime - backBtnTime;
        if(gapTime >= 0 && gapTime <= 2000){
            super.onBackPressed();
        }else{
            backBtnTime = curTime;
            Toast.makeText(this, "한번 더 누르면 종료됩니다.", Toast.LENGTH_SHORT).show();
        }

    }

}

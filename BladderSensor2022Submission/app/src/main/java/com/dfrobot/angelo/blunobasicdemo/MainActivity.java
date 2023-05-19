package com.dfrobot.angelo.blunobasicdemo;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.content.Intent;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.NotificationManagerCompat;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;
//import android.app.NotificationCompat;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import java.util.List;

public class MainActivity  extends BlunoLibrary {
	private Button buttonScan;			//bluetooth scanning button
	private Button buttonSerialSend;  //currently utilized as a testing notification button
	private EditText serialSendText;
	private TextView serialReceivedText;
	private static final String CHANNEL_ID = "channel";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		request(1000, new OnPermissionsResult() {
			@Override
			public void OnSuccess() {
				Toast.makeText(MainActivity.this, "权限请求成功", Toast.LENGTH_SHORT).show();
			}

			@Override
			public void OnFail(List<String> noPermissions) {
				Toast.makeText(MainActivity.this, "权限请求失败", Toast.LENGTH_SHORT).show();
			}
		});

		onCreateProcess();                                                        //onCreate Process by BlunoLibrary
		serialBegin(115200);                                                //set the Uart Baudrate on BLE chip to 115200
																					//both responsible for bluetooth startup

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
			CharSequence name = "channel";
			String description = "channeldescription";
			int importance = NotificationManager.IMPORTANCE_HIGH;
			NotificationChannel channel = new NotificationChannel(CHANNEL_ID, name, importance);
			channel.setDescription(description);
			// Register the channel with the system; you can't change the importance
			// or other notification behaviors after this
			NotificationManager notificationManager = getSystemService(NotificationManager.class);
			notificationManager.createNotificationChannel(channel);
		}

		//used for a simple notification, no banner notification or urgent notification
		Intent intent = new Intent(this, MainActivity.class);
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
		PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, PendingIntent.FLAG_IMMUTABLE);

		//used for more unique notification styles, banners, urgent notifications, etc., brings back to activity upon clicking notif
		Intent fullScreenIntent = new Intent(this, MainActivity.class);
		PendingIntent fullScreenPendingIntent = PendingIntent.getActivity(this, 0,
				fullScreenIntent, PendingIntent.FLAG_UPDATE_CURRENT);

		//affect the final attributes of the notification (text, icon, importance, etc.)
		final NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
				.setSmallIcon(android.R.drawable.ic_lock_idle_alarm)
				.setContentTitle("Bladder Volume Full")
				.setContentText("Impedance values detect that your patients bladder is full\\nPlease escort them to the bathroom immediately to relieve themselves.")
				.setPriority(NotificationCompat.PRIORITY_HIGH)
				.setFullScreenIntent(fullScreenPendingIntent, true);


		serialReceivedText = (TextView) findViewById(R.id.serialReveicedText);    //initial the EditText of the received data
		serialSendText = (EditText) findViewById(R.id.serialSendText);            //initial the EditText of the sending data

		buttonSerialSend = (Button) findViewById(R.id.buttonSerialSend);        //initial the button for sending the data
		buttonSerialSend.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				//Upon pressing test notif button, sleep for 5 seconds and then display the notification
				//Used to prove proof of concept that notifications work
				//in theory, can be moved to onSerialReceived or other functions within MainActivity to activate notif on impedance val instead of button click
				try {
					Thread.sleep(5000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				NotificationManagerCompat managerCompat = NotificationManagerCompat.from(MainActivity.this);
				managerCompat.notify(1, builder.build());
			}
		});

		buttonScan = (Button) findViewById(R.id.buttonScan);                    //initial the button for scanning the BLE device
		buttonScan.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub

				buttonScanOnClickProcess();                                        //Alert Dialog for selecting the BLE device
			}
		});
	}

	protected void onResume() {
		super.onResume();
		System.out.println("BlUNOActivity onResume");
		onResumeProcess();                                                        //onResume Process by BlunoLibrary
	}


	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		onActivityResultProcess(requestCode, resultCode, data);                    //onActivityResult Process by BlunoLibrary
		super.onActivityResult(requestCode, resultCode, data);
	}

	@Override
	protected void onPause() {
		super.onPause();
		onPauseProcess();                                                        //onPause Process by BlunoLibrary
	}

	protected void onStop() {
		super.onStop();
		onStopProcess();                                                        //onStop Process by BlunoLibrary
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		onDestroyProcess();                                                        //onDestroy Process by BlunoLibrary
	}

	@Override
	public void onConectionStateChange(connectionStateEnum theConnectionState) {//Once connection state changes, this function will be called
		switch (theConnectionState) {                                            //Four connection state
			case isConnected:
				buttonScan.setText("Connected");
				break;
			case isConnecting:
				buttonScan.setText("Connecting");
				break;
			case isToScan:
				buttonScan.setText("Scan");
				break;
			case isScanning:
				buttonScan.setText("Scanning");
				break;
			case isDisconnecting:
				buttonScan.setText("isDisconnecting");
				break;
			default:
				break;
		}
	}

	@Override
	public void onSerialReceived(String theString) {                            //Once connection data received, this function will be called
		// TODO Auto-generated method stub


		/*
		if(theString == "notification"){
			NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
					.setSmallIcon(R.drawable.notification_icon)
					.setContentTitle(textTitle)
					.setContentText(textContent)
					.setPriority(NotificationCompat.PRIORITY_DEFAULT);
		}*/
        /*if(scl) {
            serialReceivedText.append("SCL: ");
        }
        else{
            serialReceivedText.append(" SDA: ");
        }*/

		//serialReceivedText.append("Impedance: ");
		//serialReceivedText.append(theString.getBytes()[0]+"");
        /*String[] arrOfStr = theString.split("\\.", 4);
        byte[] bytes = {(byte)(Integer.parseInt(arrOfStr[0])),(byte)(Integer.parseInt(arrOfStr[1])),(byte)(Integer.parseInt(arrOfStr[2])),(byte)(Integer.parseInt(arrOfStr[3]))};
        float f = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN).getFloat();
        serialReceivedText.append(String.valueOf(f));*/
		//serialReceivedText.append(" SDA: ");
		//serialReceivedText.append(theString.getBytes()[1]+""); // probably not good need to change
		//serialReceivedText.append("\n");
		//append the text into the EditText
		//The Serial data from the BLUNO may be sub-packaged, so using a buffer to hold the String is a good choice.


		serialReceivedText.append(theString); //Adds the string to the print buffer
		((ScrollView)serialReceivedText.getParent()).fullScroll(View.FOCUS_DOWN); //With every new update, the list of text on screen scrolls all the way down to see whats newly printed
	}

}
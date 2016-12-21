package com.meme37.ship;

import java.io.DataOutputStream;
import java.io.IOException;

import javax.security.auth.PrivateCredentialPermission;

import com.meme37.ship.ShellUtils.CommandResult;

import android.R.color;
import android.R.integer;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.style.UpdateAppearance;
import android.view.FocusFinder;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

import android.widget.Button;
import android.widget.TextView;

public class Ship_Control extends Activity 
{
    /** Called when the activity is first created. */

    private Button mButton1;
    private Button mButton3;
    private Button mButton4;
    private Button left;
    private Button right;
    private Button network;
    private TextView val;
    private TextView mode;
    private TextView networkstate;
    public  int state1=0,state2=0;      
    public  int led_on = 1; 
	public  int led_off = 0;
	public  int fd = 0;
	public  int pwm_fd = 0;  
	public  int adc_fd = 0; 
	public  int duty = 15; 
	public  int recv = 0;
	public int[] num;
	private boolean remotemode=false;
	private boolean networkflag=false;
    private static final int COMPLETED = 0;
    private static final int OK = 1;
	private LeftThread leftThread;
	private adcThread adcThread;
	private networkThread networkThread;
	private Button.OnClickListener button4_listener= new Button.OnClickListener()
	{
		public void onClick(View v)
        {
      		remotemode=!remotemode;  
			if(remotemode){
				v.setPressed(true);
				setTitle("ON Remote mode!");
				v.setBackgroundResource(R.drawable.ledon);
				mode.setText("Remote mode");
				val.setText("X="+ 0+"Y="+0);
				
			}
			else {
				v.setPressed(false);
				setTitle("normal mode!");
				v.setBackgroundResource(R.drawable.ledoff);
				mode.setText("Normal mode");
			}
        }
	};
	private Button.OnClickListener network_listener= new Button.OnClickListener()
	{
		public void onClick(View v)
        {
      		networkflag=!networkflag;  
			if(networkflag){
				v.setPressed(true);
				if(Linuxc.opensocket()<0){
					setTitle("open Network false!");
				 
		        	finish(); 
				}
				setTitle("Netwok open");
				networkstate.setText("server waiting");
				if (networkThread==null){
					networkThread=new networkThread();
					networkThread.start();
					
				}
				
			}
			else {
				networkThread.interrupt();
				networkThread.setFlag();
				
				networkThread=null;
				v.setPressed(false);
				Linuxc.closesocket();
				setTitle("Network close");
				networkstate.setText("");
			
			}
        }
	};
	private Button.OnClickListener button3_listener= new Button.OnClickListener(){
		 public void onClick(View v)
         {
          
       	   finish();
           }
	};
	 private class onTouchListener implements OnTouchListener
	    {
		 @Override
		    public boolean onTouch(View arg0, MotionEvent arg1) {
			
			 boolean charge=false;
			 if(arg1.getAction()==MotionEvent.ACTION_MOVE)
				return true;
			 LeftThread leftThreadold=leftThread;
			 leftThread=new LeftThread();
			 
			 switch (arg0.getId()) {
			 	case R.id.button1:
			 		leftThread.setLeftFlag();
			 		break;
			 	case R.id.button2:
				
			 		break;
			 	case R.id.led_myButton1:
			 		charge=true;
			 		break;
			 	default:
				break;
			 }
			 if(arg1.getAction()==MotionEvent.ACTION_DOWN){
				
				 arg0.setPressed(true);
	
				if (charge) {
					Linuxc.send(1, led_on);
				}else	
					leftThread.start();
				 
		      }
			 if(arg1.getAction()==MotionEvent.ACTION_UP){
				 arg0.setPressed(false);
				 if(charge){
					 Linuxc.send(1, led_off);
				 }else{
				 	leftThreadold.setFlag(); 
				 	leftThreadold.interrupt();
				 	leftThreadold=null;
				 }
			 }
			 return true;
		 }
	    }
	  private Handler handler = new Handler() {  
	        @Override  
	        public void handleMessage(Message msg) {  
	            if (msg.what == COMPLETED) {  
	            	
	            	if(remotemode==true){
	            		
	            	}else{
	            		val.setText("X="+ num[0]+"Y="+num[1]);
	            		right.setPressed(false);
	            		
	            		left.setPressed(false);
	            		
	            		if(num[1]>2000){
	            			right.setPressed(true);
	    				 
	            			if(duty<20){
	            				duty++;
	            				Linuxc.setpwm(duty);
	            			}
						 
	            		}
	            		if(num[1]<700){
	            			left.setPressed(true);
	            		//	left.setFocusableInTouchMode(true);
	            	     //	left.requestFocusFromTouch();
	    				 
	            	     	if(duty>9){
	            	     		duty--;
	            	     		Linuxc.setpwm(duty);
	            	     	}
	            		}
	            		if(num[0]<700){
	            			mButton1.setPressed(true);
	            			Linuxc.send(1, led_on);
	            		}else{
	            			
	            			mButton1.setPressed(false);
	            			Linuxc.send(1, led_off);
	            		}
	            	}
	            	
	            }
	            if(msg.what==OK){
	            	switch(recv){
	            		case 0:
	            			Linuxc.send(1, led_off);
	            			break;
	            		case 1:
	            			Linuxc.send(1, led_on);
	            			break;
	            		default:
	            			Linuxc.setpwm(recv);
	            			break;
	            	
	            	}
	            }
	        }  
	    }; 


	    
	    
	    public class networkThread extends Thread{
			 private volatile boolean flag=true;
			 private int client=0;
			 public void setFlag(){
				 	flag=false;
			 }
			 public boolean getFlag(){
				 	return flag;
			 }
			 public void run(){
				System.out.println("waiting");
				 while(getFlag()==true){
					 try {
						 
							while(client<=0&&getFlag()==true){
								client=Linuxc.connectclient();
								
							}
					} catch (Exception e) {
						// TODO: handle exception
						System.out.println( "accept() failed or interrupted..." );
					}
					 
					
					recv=Linuxc.readclient();
					 
					Message msg = new Message();  
		            msg.what = OK;  
		            handler.sendMessage(msg);
		            
				 }
			 }
		 
		 }
	 public class adcThread extends Thread{
		 private volatile boolean flag=true;
		 public void setFlag(){
			 	flag=false;
		 }
		 public boolean getFlag(){
			 	return flag;
		 }
		 public void run(){
			 while(getFlag()==true){
				 if(remotemode==false){
					 num=Linuxc.readadc();
					 Message msg = new Message();  
					 msg.what = COMPLETED;  
					 handler.sendMessage(msg);
				 }
			 }
		 }
	 
	 }
	 public class LeftThread extends Thread{
		 private volatile boolean flag =true;
		 private volatile boolean leftflag =true;
		 public void setFlag(){
			 	flag=false;
			 	System.out.println("flag=" + getFlag() ); 
		 }
		 public boolean getFlag(){
			 
			 	return flag;
		 }
		 public void setLeftFlag(){
			 	leftflag=false;
			 	System.out.println("leftflag=" + getLeftFlag() ); 
		 }
		 public boolean getLeftFlag(){
			 
			 	return leftflag;
		 }
		 public void run(){
			 while (getFlag()== true) {
				 System.out.println("nowflag=" + getFlag()  ); 
				 System.out.println("duty=" + duty  ); 
				
				 if(getLeftFlag()==true){
					 duty++;
					 if(duty>=20)
						 break;
				 }
				 else {
					 duty--;
					 if(duty<=9){
						 break;
					 }
				 }
		    	 Linuxc.setpwm(duty);
		    	
					try {
						sleep(100,0);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						   System.out.println("I am interrupted...."); 
					}	
			}
			   
		 }
	 }
	 
    @Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
    	 Linuxc.closeled();
         adcThread.setFlag(); 
         adcThread.interrupt();
     	 adcThread=null;
     	if(networkThread!=null){ 
     		networkThread.setFlag(); 
     		networkThread.interrupt();
			networkThread=null;
		}
		super.onDestroy();
		
	}

	@Override
    public void onCreate(Bundle savedInstanceState) {
		CommandResult Command= ShellUtils.execCommand("sh /system/bin/install.sh", true);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
     
        mButton1=(Button)findViewById(R.id.led_myButton1);
        mButton3 =(Button) findViewById(R.id.led_myButton3);
        mButton4 =(Button) findViewById(R.id.led_myButton1_1);
        left=(Button)findViewById(R.id.button1);
        right=(Button)findViewById(R.id.button2);
        network=(Button)findViewById(R.id.internet);
        fd = Linuxc.openled();
        val=(TextView)findViewById(R.id.textView1);
        mode=(TextView)findViewById(R.id.led_view1);
        networkstate=(TextView)findViewById(R.id.textView2);
       
        if (fd < 0){
        	setTitle("open device false!");
     
        	finish(); 
        }
        else {
        	setTitle("open device success!");
        }
        pwm_fd = Linuxc.openpwm();
        
        if (pwm_fd < 0){
        	setTitle("open pwm false!");
     
        	finish(); 
        }
        else {
        	setTitle("open pwm success!");
        }
        adc_fd = Linuxc.openadc();
        
        if (adc_fd < 0){
        	setTitle("open adcfalse!");
     
        	finish(); 
        }
        else {
        	setTitle("open adc success!");
        }
        mButton1.setOnTouchListener(new onTouchListener());
        mButton3.setOnClickListener(button3_listener);
        mButton4.setOnClickListener(button4_listener);
        network.setOnClickListener(network_listener);
        left.setOnTouchListener(new onTouchListener());
        right.setOnTouchListener(new onTouchListener());
        
        Linuxc.setpwm(duty);
        num=Linuxc.readadc();
        val.setText("X="+ num[0]+"Y="+num[1]);
        adcThread=new adcThread();
        adcThread.start();
        Command=null;
    }
}
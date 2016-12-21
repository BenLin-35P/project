package com.meme37.ship;

import android.util.Log;

public class Linuxc {
	static {
        try {
            Log.i("JNI", "Trying to load libtotal.so");

            System.loadLibrary("total"); 
        }
        catch (UnsatisfiedLinkError ule) {
            Log.e("JNI", "WARNING: Could not load libtotal.so");
        }}

	public static native int openled();
	public static native int openpwm();
	public static native int openadc();
	public static native int opensocket();
	public static native int closesocket();
	public static native int connectclient();
	public static native int readclient();
	public static native int closeled();
	public static native void stoppwm();
	public static native void setpwm(int duty);
	public static native int[] readadc();
	public static native int send(int led_num, int on_off);
}

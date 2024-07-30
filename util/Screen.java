package connectivity;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.UnknownHostException;

import android.app.ActivityThread;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.IpPrefix;
import android.net.LinkAddress;
import android.net.LinkProperties;
import android.net.NetworkAgent;
import android.net.NetworkAgentConfig;
import android.net.NetworkCapabilities;
import android.net.NetworkProvider;
import android.net.NetworkProvider.NetworkOfferCallback;
import android.net.NetworkRequest;
import android.net.NetworkScore;
import android.net.RouteInfo;
import android.os.Handler;
import android.os.Looper;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;

public class Screen {
	public static void main(String... args) throws UnknownHostException {
		Looper.prepareMainLooper();
		ActivityThread thread = ActivityThread.systemMain();
		Context context = thread.getSystemContext();
		Looper looper = Looper.getMainLooper();
		Handler handler = new Handler(looper);

		PowerManager powerManager = (PowerManager) context.getSystemService(context.POWER_SERVICE);
		WakeLock wakeLock = powerManager.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK,
				        "MyApp::MyWakelockTag");
		wakeLock.acquire();


                /*
		long HOURS = 1000 * 60 * 60;
		handler.postDelayed(() -> {
			System.exit(0);
		}, 3 * HOURS);
                */

		Looper.loop();
	}
}


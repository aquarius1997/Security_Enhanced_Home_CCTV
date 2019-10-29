import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.io.IOException;

public class IBSClient {
	static {
		System.loadLibrary("IBSModule2");
	}

	private native String GetID();
	private native String GetRand();
	private native String Sign(String msg, int msglen);
	private native boolean Verify(String msg, int msglen, String chal, int challen);
	private native String Hash(String msg, int msglen);
	private native String AES_GCM_dec(String ciphertext, int textlen, String key);
	private native String AES_GCM_enc(String Plaintext, int txtlen, String SessionKey);
	public static final int ServerPort = 5000;
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		ServerSocket serverSocket = null;
		Socket client = null;
		BufferedReader in = null;
		PrintWriter out = null;
		Process raspivid = null;

	for(;;){
		try {
				serverSocket = new ServerSocket(ServerPort);
				
				System.out.println("Waiting for clients...");
				client = serverSocket.accept();
				in = new BufferedReader(new InputStreamReader(client.getInputStream()));
				out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(client.getOutputStream())), true);

				IBSClient IBS = new IBSClient();
				String pw = "temp";
				String rand_server = "";
				String rand_client = "";
				String id_server = "";
				String id_client = "";
				String temp_message = "";
				String send_message = "";
				String receive_message = "";
				String server_message = "";
				String server_sign = "";
				String send_message2 = "";
				String verify_message = "";
				String client_sign = "";

				int updownDegree = 15;
				int leftrightDegree = 15;

				id_server = IBS.GetID();
					System.out.println("1 " + id_server);

				rand_server = IBS.GetRand();
				System.out.println("2 " + rand_server + " len : " + rand_server.length());
				
				receive_message = in.readLine();
				id_client = receive_message.substring(0, 8);
				rand_client = receive_message.substring(8, 8+64);
				
				System.out.println("id_client : " + id_client + "length : " + id_client.length());
				System.out.println("rand_client : " + rand_client + "length : "+ rand_client.length());
				System.out.println("testtest");
				server_message = id_server + receive_message + rand_server + pw;
				System.out.println("3 : server_message " + server_message);

				server_sign = IBS.Sign(server_message, server_message.length());
				//System.out.println("4 : Sign " + server_sign);
				send_message2 = server_sign + rand_server;
							
				out.println(send_message2);
				
				client_sign = in.readLine();
				verify_message = server_message + server_sign;

				if(IBS.Verify(client_sign, client_sign.length(), verify_message, verify_message.length())){

				System.out.println("Verified!!!");

				String strHash = "";
				String strHash2 = "";
				strHash = id_client + id_server + rand_client + rand_server;

				strHash2 = IBS.Hash(strHash,strHash.length());	

				String GstreamerKey = strHash2.substring(0,60);

				String ipAddressRecv = in.readLine();
				
				String ipAddress = IBS.AES_GCM_dec(ipAddressRecv, ipAddressRecv.length(), strHash2);
				

				System.out.println("Session Key is : " + strHash2 + "Streamer key is : " + GstreamerKey + "IP is : " + ipAddress);

				String cmd = "raspivid -t 0 -h 480 -w 640 -fps 15 -b 200000 -o -  | gst-launch-1.0 fdsrc ! h264parse ! rtph264pay config-interval=1 pt=96 ! 'application/x-rtp, payload=(int)96, ssrc=(uint)3412089386' ! srtpenc key=\""+ GstreamerKey  +"\" ! udpsink host="+ ipAddress +" port=5004";

				String[] command = {
					"/bin/sh",
					"-c",
					cmd
				};	

				raspivid = Runtime.getRuntime().exec(command);
				for(;;){
					String recv, temp;
					recv = in.readLine();
					System.out.println(recv);
					temp = IBS.AES_GCM_dec(recv, recv.length(), strHash2);
					System.out.println(temp);

					if(temp.equals("UP"))
					{
						if(updownDegree > 5)
							Runtime.getRuntime().exec("./servo 2 " + (--updownDegree));
					}
					else if(temp.equals("DOWN"))
					{
						if(updownDegree < 24)
							Runtime.getRuntime().exec("./servo 2 " + (++updownDegree));
					}
					else if(temp.equals("LEFT"))
					{
						if(leftrightDegree < 24)
							Runtime.getRuntime().exec("./servo 1 " + (++leftrightDegree));
					}
					else if(temp.equals("RIGHT"))
					{
						if(leftrightDegree > 5)
							Runtime.getRuntime().exec("./servo 1 " + (--leftrightDegree));
					}
				}
			}
			else{
				System.out.println("FAILURE!");
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			System.out.println(raspivid);
			try{
			Runtime.getRuntime().exec("pkill raspivid");
			out.close();
			in.close();
			client.close();
			serverSocket.close();
			}
			catch(IOException ex){
			}
		}

	}
	}

}

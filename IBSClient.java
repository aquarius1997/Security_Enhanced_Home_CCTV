import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.io.IOException;
import java.net.InetAddress;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.NetworkInterface;
import java.util.Enumeration;
import java.net.*;

public class IBSClient {
	static {
		System.loadLibrary("IBSModule2");
	}

	private native String GetIV();
	private native String GetID();
	private native String GetRand();
	private native String getGMod(String exp);
	private native String getBaseExp(String base, String exp);
	private native String Sign(String msg, int msglen);
	private native boolean Verify(String msg, int msglen, String chal, int challen);
	private native String Hash(String msg, int msglen);
	private native String AES_GCM_dec(String ciphertext, int textlen, String key, String RandIV);
	private native String AES_GCM_enc(String Plaintext, int txtlen, String SessionKey, String RandIV);
	public static final int ServerPort = 5000;
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		ServerSocket serverSocket = null;
		Socket client = null;
		BufferedReader in = null;
		PrintWriter out = null;
		Process raspivid = null;
		FindIP findip = new FindIP();
		findip.start();
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
				String Diff_Hell_Client = "";
				String Diff_Hell_Server = "";
				String K1 = "";
				String K2 = "";
				String Diff_Hell_Key = "";
				String rand_client = "";
				String id_server = "";
				String id_client = "";
				String temp_message = "";
				String server_message = "";
				String receive_message = "";
				String client_message = "";
				String server_sign = "";
				String send_message2 = "";
				String verify_message = "";
				String client_sign = "";
				String getGMod_result = "";
				String IV = "";
				String Hashed_Diff_Hell_Key = "";
				String K1_Enc = "";
				String ID_PW = "";
				String hashed_ssk = "";
				int updownDegree = 15;
				int leftrightDegree = 15;

				IBS.Sign("temp", "temp".length());
				id_server = IBS.GetID();
					System.out.println("1 " + id_server);

				rand_server = IBS.GetRand();
				System.out.println("2 " + rand_server + " len : " + rand_server.length());

				Diff_Hell_Server = IBS.getGMod(rand_server);
				out.println(Diff_Hell_Server);		
				System.out.println("3. Diff_Hell_Server :  " + Diff_Hell_Server);
				
				receive_message = in.readLine();		//receive_message = ID || Diff_Hell_Client || signature(Client) || IV
				id_client = receive_message.substring(0, 8);
				Diff_Hell_Client = receive_message.substring(8,receive_message.length()-584);
				client_sign = receive_message.substring(receive_message.length()-584, receive_message.length()-8);
				IV = receive_message.substring(receive_message.length()-8, receive_message.length());

				System.out.println("ID : " + id_client + ", Diff_Hell_Client : " + Diff_Hell_Client + ", client_sign : " + client_sign + ", IV : " + IV);

				Diff_Hell_Key = IBS.getBaseExp(Diff_Hell_Client, rand_server);
				System.out.println("All EXP Result : " + Diff_Hell_Key);

				Hashed_Diff_Hell_Key = IBS.Hash(Diff_Hell_Key,Diff_Hell_Key.length());	
				K1 = Hashed_Diff_Hell_Key.substring(0, Hashed_Diff_Hell_Key.length()/2);
				K2 = Hashed_Diff_Hell_Key.substring(Hashed_Diff_Hell_Key.length()/2, Hashed_Diff_Hell_Key.length());
				System.out.println("key 1 : " + K1 + ", Key 2 : " + K2);

				ID_PW = id_server + pw;
				K1_Enc = IBS.AES_GCM_enc(ID_PW, ID_PW.length(), K1, IV);
				
				client_message = id_client + Diff_Hell_Server + Diff_Hell_Client + K1_Enc;

				System.out.println("k1_enc : " + K1_Enc + ", client_message : " + client_message);

				if(IBS.Verify(client_sign, client_sign.length(), client_message, client_message.length())){

				System.out.println("Verified!!!");

				String server_IV = IBS.GetIV();
				String server_K1_Enc = IBS.AES_GCM_enc(ID_PW, ID_PW.length(), K1, server_IV);
				

				server_message = id_client + Diff_Hell_Server + Diff_Hell_Client + server_K1_Enc + client_sign;

				server_sign = IBS.Sign(server_message, server_message.length());
				out.println(server_sign+server_IV);

				String ssk_msg = id_client + Diff_Hell_Server + Diff_Hell_Client + K2;
				hashed_ssk = IBS.Hash(ssk_msg,ssk_msg.length());	


				/*asdf
				//System.out.println("test exponent : " + IBS.getGMod(null, rand_server));
				//ok???? gogo??? gogo
				System.out.println("id_client : " + id_client + "length : " + id_client.length());
				System.out.println("rand_client : " + rand_client + "length : "+ rand_client.length());

				System.out.println("result1 : " + IBS.getBaseExp(IBS.getGMod(rand_client), rand_server));

				System.out.println("result2 : " + IBS.getBaseExp(IBS.getGMod(rand_server),rand_client));

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
				
			asdf	*/
				String GstreamerKey = hashed_ssk.substring(0,60);

				String ipAddressRecv = in.readLine();
		
				System.out.println(ipAddressRecv + "size : " + ipAddressRecv.length());

				String ipIV = ipAddressRecv.substring(ipAddressRecv.length()-8, ipAddressRecv.length());

				ipAddressRecv = ipAddressRecv.substring(0, ipAddressRecv.length()-8);

				String ipAddress = IBS.AES_GCM_dec(ipAddressRecv, ipAddressRecv.length(), hashed_ssk, ipIV);

				System.out.println("Session Key is : " + hashed_ssk + "\nGstreamer key is : " + GstreamerKey + "\nClient IP is : " + ipAddress);

				String cmd = "raspivid -t 0 -h 480 -w 640 -fps 10 -b 200000 -o -  | gst-launch-1.0 fdsrc ! h264parse ! rtph264pay config-interval=1 pt=96 ! 'application/x-rtp, payload=(int)96, ssrc=(uint)3412089386' ! srtpenc key=\""+ GstreamerKey  +"\" ! udpsink host=" + ipAddress  + " port=5004";

				String[] command = {
					"/bin/sh",
					"-c",
					cmd
				};	

				raspivid = Runtime.getRuntime().exec(command);

				//String plaintxt = "123456789Hello";
				//String ciphertxt = "";
				//ciphertxt = IBS.AES_GCM_enc(plaintxt, plaintxt.length(), strHash2);
				//out.println(ciphertxt);

				for(;;){
					String recv, temp;
					recv = in.readLine();
					System.out.println(recv);
					String recvIV = recv.substring(recv.length()-8, recv.length());
					temp = IBS.AES_GCM_dec(recv, recv.length(), hashed_ssk, recvIV);
					System.out.println(temp);

					if(temp.substring(0,4).equals("UPUP"))
					{
						if(updownDegree > 5)
							Runtime.getRuntime().exec("./servo 2 " + (--updownDegree));
					}
					else if(temp.substring(0,4).equals("DOWN"))
					{
						if(updownDegree < 24)
							Runtime.getRuntime().exec("./servo 2 " + (++updownDegree));
					}
					else if(temp.substring(0,4).equals("LEFT"))
					{
						if(leftrightDegree < 24)
							Runtime.getRuntime().exec("./servo 1 " + (++leftrightDegree));
					}
					else if(temp.substring(0,4).equals("RIGH"))
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


class FindIP extends Thread{
	public void run(){
		String myIP = "";
		NetworkInterface ni = null;
		DatagramSocket socket = null;
		try{
		ni = NetworkInterface.getByName("wlan0");
		}catch(SocketException ex){}
		Enumeration<InetAddress> inetAddresses = ni.getInetAddresses();
		InetAddress ia = inetAddresses.nextElement();
		ia = inetAddresses.nextElement();
		System.out.println("IP : " + ia.getHostAddress());
		myIP = ia.getHostAddress();

		try{
			InetAddress addr = InetAddress.getByName("0.0.0.0");
			socket = new DatagramSocket(5005, addr);
			socket.setBroadcast(true);
		}catch(Exception e){
			System.out.println("Could not open UDP");
		}

		for(;;){
			byte[] recvBuf = new byte[2048];
			DatagramPacket packet = new DatagramPacket(recvBuf, recvBuf.length);
			try{
				socket.receive(packet);
			}catch (IOException ie){
				System.out.println("UDP catch error");
			}

			InetAddress clientAddress = packet.getAddress();
			int clientPort = packet.getPort();

			String message = new String(packet.getData()).trim();

			if (message.startsWith("YEC-CAM-FIND")){
				System.out.println("CAM FIND UDP Packet From : " + clientAddress.getHostAddress() + ":" + clientPort);
				String reply = "YEC_IP_IS"+myIP;
				byte[] sendData = reply.getBytes();
				
				DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, clientAddress, clientPort);

				try {
					socket.send(sendPacket);
				}catch(IOException ie){
					System.out.println("send error");
				}
			}
		}
	}
}

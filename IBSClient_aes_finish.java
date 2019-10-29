import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;


public class IBSClient {
	static {
		System.loadLibrary("IBSModule2");
	}

	private native String GetID();
	private native String GetRand();
	private native String Sign(String msg, int msglen);
	private native boolean Verify(String msg, int msglen, String chal, int challen);
	private native String Hash(String msg, int msglen);
	private native String AES_GCM_enc(String Plaintext, int txtlen, String SessionKey);
	public static final int ServerPort = 5000;
    public static final String ServerIP = "1.239.213.150";
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try {
			ServerSocket serverSocket = new ServerSocket(ServerPort);
			
			System.out.println("Waiting for clients...");
			Socket client = serverSocket.accept();
			BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
			PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(client.getOutputStream())), true);

			IBSClient IBS = new IBSClient();
			
			String rand_server = "";
			String rand_client = "";
			String id_server = "";
			String id_client = "";
			String temp_message = "";
			String send_message = "";
			String receive_message = "";
			
			id_server = IBS.GetID();
		    	System.out.println("1 " + id_server);

			rand_server = IBS.GetRand();
			System.out.println("2 " + rand_server + " len : " + rand_server.length());
			
			id_client = in.readLine();
			System.out.println("3 " + id_client);
			
			send_message = id_server + rand_server;
			out.println(send_message);
			System.out.println("4 " + send_message);
			
			receive_message = in.readLine();
			System.out.println("5 : " + receive_message);
			
			rand_client = receive_message.substring(0, 64); //R_client ;;
			System.out.println("6 : " + rand_client + "length : " + rand_client.length());
			
			String receive_message2 = receive_message.substring(64, receive_message.length());
			System.out.println("7 : " + receive_message2 + "--> " +receive_message2.length());
			
			
			
			temp_message = id_client + id_server + rand_server;
			System.out.println("10 : " + temp_message+" -->len: "+temp_message.length());
			
			send_message = temp_message + rand_client;
			String send_sign = IBS.Sign(send_message, send_message.length());
			out.println(send_sign);
			System.out.println("8 server_sign: " + send_sign + " len : " + send_sign.length());
			
			//String vrfy = IBS.Verify(receive_message2, receive_message2.length(), temp_message, temp_message.length());
			//System.out.println("9 vrfy: " + vrfy + " len"+ vrfy.length());
			if(!(IBS.Verify(receive_message2, receive_message2.length(), temp_message, temp_message.length()))) {
				System.out.println("Not Verified!!");
				
				//return;
			}
			
			System.out.println("Verified!!!");

			String strHash = "";
			String strHash2 = "";
			strHash = id_client + id_server + rand_client + rand_server;

			strHash2 = IBS.Hash(strHash,strHash.length());	

			System.out.println("Session Key is : " + strHash2);

			String plaintxt = "123456789Hello";
			String ciphertxt = "";
			ciphertxt = IBS.AES_GCM_enc(plaintxt, plaintxt.length(), strHash2);
			System.out.println("Test Ciphertxt :" + ciphertxt);
			out.println(ciphertxt);

			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}

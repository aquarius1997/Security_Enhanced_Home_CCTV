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

			id_server = IBS.GetID();
		    	System.out.println("1 " + id_server);

			rand_server = IBS.GetRand();
			System.out.println("2 " + rand_server + " len : " + rand_server.length());
			
			receive_message = in.readLine();

			server_message = id_server + receive_message + rand_server + pw;
			server_sign = IBS.Sign(server_message, server_message.length());
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

			System.out.println("Session Key is : " + strHash2);

			String plaintxt = "123456789Hello";
			String ciphertxt = "";
			ciphertxt = IBS.AES_GCM_enc(plaintxt, plaintxt.length(), strHash2);
			System.out.println("Test Ciphertxt :" + ciphertxt);
			out.println(ciphertxt);
			}
			else{
				System.out.println("FAILURE!");
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}

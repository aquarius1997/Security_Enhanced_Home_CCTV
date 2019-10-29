import java.net.InetSocketAddress;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;

public class IBSClient {
	
	static {
		System.loadLibrary("IBSModule2");
	}

	private native String GetID();
	private native String GetRand();
	private native String Sign(String msg, int msglen);
	private native boolean Verify(String msg, int msglen, String chal, int challen);
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try {
			ServerSocketChannel server = ServerSocketChannel.open();
			server.socket().bind(new InetSocketAddress(5000));
			
			System.out.println("Waiting for clients...");
			SocketChannel client = server.accept();
			
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
			
			id_client = HelperMethods.receiveMessage(client);
			System.out.println("3 " + id_client);
			
			send_message = id_server + rand_server;
			HelperMethods.sendMessage(client, send_message);
			System.out.println("4 " + send_message);
			
			receive_message = HelperMethods.receiveMessage(client);
			System.out.println("5 : " + receive_message);
			
			rand_client = receive_message.substring(0, 64); //R_client ;;
			System.out.println("6 : " + rand_client + "length : " + rand_client.length());
			
			String receive_message2 = receive_message.substring(64, receive_message.length());
			System.out.println("7 : " + receive_message2 + "--> " +receive_message2.length());
			
			
			
			temp_message = id_client + id_server + rand_server;
			System.out.println("10 : " + temp_message+" -->len: "+temp_message.length());
			
			send_message = temp_message + rand_client;
			String send_sign = IBS.Sign(send_message, send_message.length());
			HelperMethods.sendMessage(client, send_sign);
			System.out.println("8 server_sign: " + send_sign + " len : " + send_sign.length());
			
			//String vrfy = IBS.Verify(receive_message2, receive_message2.length(), temp_message, temp_message.length());
			//System.out.println("9 vrfy: " + vrfy + " len"+ vrfy.length());
			if(!(IBS.Verify(receive_message2, receive_message2.length(), temp_message, temp_message.length()))) {
				System.out.println("Not Verified!!");
				
				return;
			}
			
			System.out.println("Verified!!!");
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

public class HelperMethods {

    public static void sendMessage(SocketChannel socketChannel, String message) {
	try {
	    ByteBuffer buffer = ByteBuffer.allocate(message.length()+1);
	    buffer.put(message.getBytes());
	    buffer.put((byte)0x00);
	    buffer.flip();
	    while(buffer.hasRemaining()) {
		socketChannel.write(buffer);
	    }
	} catch (IOException ex) {
	    ex.printStackTrace();
	}
    }

    public static String receiveMessage(SocketChannel socketChannel) {
	String message = "";
	try {
	    ByteBuffer byteBuffer = ByteBuffer.allocate(64);

	    while(socketChannel.read(byteBuffer)>0) {
		char byteRead = 0x00;
		byteBuffer.flip();

		while(byteBuffer.hasRemaining()) {
		    byteRead = (char) byteBuffer.get();
		    if(byteRead != 0x00) {
			message += byteRead;
		    } else {
			break;
		    }
		}
		if(byteRead==0x00) {
		    break;
		}
		byteBuffer.clear();
	    }
	} catch (IOException ex) {
	    ex.printStackTrace();
	}
	return message;
    }
}

import java.awt.SecondaryLoop;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketTimeoutException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public class ClientUdp {
	private final String serverDomainName = "uw.umx.io";
	private final String serverIP ="73.140.72.152";
	private final int serverPortNumber = 15002;
	private final int MAX_RETRY_TIMES = 3;
	
	public String UdpTest(){
		try{
			DatagramSocket clientSocket = new DatagramSocket();
			InetAddress ipAddress = InetAddress.getByName(serverIP);
			
			String json = "{\"jsonkv\":\"1.0\",\"operate\":\"put\",\"key\":\"apple\",\"value\":\"www.apple.com\",\"id\":\"1\"}";
			final ByteArrayOutputStream array_os = new ByteArrayOutputStream();
			final DataOutputStream os = new DataOutputStream(array_os);
			os.writeInt(0);
			os.writeInt(0);
			os.writeInt(16 + json.length());
			os.writeInt(0);
			os.writeBytes(json);
			final byte[] data = array_os.toByteArray();
			os.close();
			
			DatagramPacket sendPacket = new DatagramPacket(data, data.length, ipAddress,serverPortNumber);
			byte[] receiveData = new byte[128];
			clientSocket.send(sendPacket);
			long startTime = System.currentTimeMillis();
			
			clientSocket.setSoTimeout(3000);
			for(int i=0; i < MAX_RETRY_TIMES; i++){
				DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
				try{
					clientSocket.receive(receivePacket);
					long endTime = System.currentTimeMillis();
					long duration = (endTime - startTime);
					return (new String(receivePacket.getData()));
				}catch (SocketTimeoutException e){
					//resend;
					clientSocket.send(sendPacket);
					continue;
				}
			}
			clientSocket.close();
		}catch(Exception ex){
			System.out.print("exception while doing udp call");
			ex.printStackTrace();
		}finally{
			return null;
		}
				
	}

}

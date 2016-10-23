import java.io.*;
import java.net.*;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ClientUdp {
	private final String severIp;
	private final int serverPortNumber;
	private final int MAX_RETRY_TIME = 3;
    private final Logger logger;

	public ClientUdp(String severIp, int port) {
		this.severIp = severIp;
		this.serverPortNumber = port;
        this.logger = LoggerFactory.getLogger(ClientUdp.class);
	}

	public String UdpTest(String json){
		try{
			DatagramSocket clientSocket = new DatagramSocket();
			InetAddress ipAddress = InetAddress.getByName(severIp);
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
			clientSocket.setSoTimeout(3000);

			for(int i = 0; i < MAX_RETRY_TIME; i++){
				DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
				try{
					clientSocket.receive(receivePacket);
					return new String(receivePacket.getData());
				}catch (SocketTimeoutException e){
					//resend
					clientSocket.send(sendPacket);
					continue;
				}
			}
			clientSocket.close();
		}catch(Exception ex){
			System.out.print("exception while doing udp call");
            logger.info("exception while doing udp call",
                    new SimpleDateFormat("yyyy/MM/dd HH:mm:ss").format(new Date()));
			ex.printStackTrace();
		}
		return null;
	}

}

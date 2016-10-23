import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.*;
import java.net.*;
import java.text.SimpleDateFormat;
import java.util.Date;

public class ClientTcp {
	private final String severIp;
	private final int serverPortNumber;
	private final Logger logger;

	public ClientTcp(String severIp, int port) {
		this.severIp = severIp;
		this.serverPortNumber = port;
		this.logger = LoggerFactory.getLogger(ClientTcp.class);
	}

	public String TcpTest(String json) {
		try{
			Socket clientSocket = new Socket(severIp, serverPortNumber);
			DataOutputStream os = new DataOutputStream(clientSocket.getOutputStream());
			DataInputStream is = new DataInputStream(clientSocket.getInputStream());
			
			os.writeInt(0);
			os.writeInt(0);
			os.writeInt(16+json.length());
			os.writeInt(0);
			os.writeBytes(json);
			os.flush();
			
			BufferedReader in = new BufferedReader(new InputStreamReader(is));
			String response = "";
			String inputLine = "";
			while ((inputLine = in.readLine()) != null){
				response += inputLine;
				//System.out.println(response);
			}
			in.close();
			os.close();
			is.close();
			clientSocket.close();
			return response;
		} catch (Exception ex) {
			System.out.print("exception while doing tcp call");
			ex.printStackTrace();
			logger.info("exception while doing tcp call",
					new SimpleDateFormat("yyyy/MM/dd HH:mm:ss").format(new Date()));
			return null;
		}
	}

}

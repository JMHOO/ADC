import java.io.*;
import java.net.*;

public class ClientTcp {
	private final String serverDomainName = "uw.umx.io";
	private final String serverIP ="73.140.72.152";
	private final int serverPortNumber = 15001;
	
	public void TcpTest(String json) {
		try{
			Socket clientSocket = new Socket(serverIP, serverPortNumber);
			DataOutputStream os = new DataOutputStream(clientSocket.getOutputStream());
			DataInputStream is = new DataInputStream(clientSocket.getInputStream());
			
			os.writeInt(0);
			os.writeInt(0);
			os.writeInt(16+json.length());
			os.writeInt(0);
			os.writeBytes(json);
			os.flush();
			
			BufferedReader in = new BufferedReader(new InputStreamReader(is));
			String inputLine;
			while ((inputLine = in.readLine()) != null){
				System.out.println(inputLine);
			}
			in.close();
			os.close();
			is.close();
			clientSocket.close();
		} catch (Exception ex) {
			System.out.print("exception while doing tcp call");
			ex.printStackTrace();
		}
	}

}

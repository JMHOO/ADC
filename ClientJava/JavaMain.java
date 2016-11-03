import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.List;
import java.util.Scanner;
import java.util.Vector;



public class JavaMain {
	private static String serverDomainName = "uw.umx.io";
	private static String serverIP ="73.140.72.152";
	private static String serverPortNumber = "15001";
	private static Vector<String> hostList = new Vector<String>();

	private static String inputFile = "kvp-operations.csv";
	private static final String outputFileTcp = "output_tcp.csv";
	
	private final static OperateCSV csv = new OperateCSV();
	// IO streams
	private static DataOutputStream toServer;
	private static DataInputStream fromServer;
	
    public static void main(String[] args) throws Throwable {  
    	// create a Scanner object
		Scanner input = new Scanner(System.in);
    	String response, inputLine;
    	
		try {
			if (args.length == 3) {
    			serverIP = args[0];
    			serverPortNumber= args[1];
    			inputFile = args[2];
    		}
    		else if (args.length == 2) {
    			serverIP = args[0];
    			serverPortNumber= args[1];
    		}
    		else {
    			// prompt the user to enter the serverIP
				System.out.println("Please enter the server IP or name: ");		 /** Server name/IP 处理 */
				serverIP = input.nextLine();
				// prompt the user to enter the port number
				System.out.println("Please enter the server port number: ");
				serverPortNumber = input.nextLine();
    		}
			
			//service discovery
			serviceDiscover();
			
			// execute operations
			System.out.println("execute");
			executeOperation();
			
			
    	}
		catch(Exception ex) {
			ex.printStackTrace();
		}
		input.close();
    }
    
    public static void serviceDiscover() throws NumberFormatException, UnknownHostException, IOException {
    	// Create socket
    	Socket socket = new Socket(serverIP, Integer.parseInt(serverPortNumber));
    	// Create input stream to receive data
    	fromServer = new DataInputStream(socket.getInputStream());
    	// Create output stream to send data
    	toServer = new DataOutputStream(socket.getOutputStream());
    	
    	// build json string to get server list
    	String jsonRequest = "{\"jsonagent\":\"1.0\",\"operate\":\"getserverlist\",\"protocol\":\"tcp\"}";
    	
    	// protocol
		toServer.writeInt(0);
		toServer.writeInt(0);
		toServer.writeInt(16 + jsonRequest.length());
		toServer.writeInt(0);
		toServer.writeBytes(jsonRequest);
		toServer.flush();
		
		// read response
		byte[] header = new byte[16];
		fromServer.read(header, 0, 16);
		
		// where magic happens
		int len = (int) ((((header[8] & 0xff) << 24) | ((header[9] & 0xff) << 16) | ((header[10] & 0xff) << 8) | ((header[11] & 0xff) << 0)));    
		byte[] message = new byte[len];
		fromServer.read(message, 0, len);
		System.out.println(new String(message));
		
		// phrase json
		String[] a = new String(message).split("[\\[\\]]");
//		System.out.println(a[1]);
		String[] b = a[1].split("address\":\"");
		for (int i = 1; i < b.length; i++) {
			String[] c = b[i].split("\",\"");
			String[] d = c[1].split("port\"");
			String[] e = d[1].split("}");
			StringBuffer sb = new StringBuffer();
			sb.append(c[0]).append(e[0]);
			// add to the host list
			hostList.add(sb.toString());		// format: ip_address:port_number
		}
    }
    
    
    // Execute operations
    public static void executeOperation() throws NumberFormatException, UnknownHostException, IOException {
		String server, port;
		String [] temp;
		
		// pop the first host address from the service list
		temp = hostList.firstElement().split(":");
		server = temp[0];
		port = temp[1];
		System.out.println("Server: " + server + ", port: " + port);
		
		// Get random 
		
    	// Create socket
		Socket socket = new Socket(server, Integer.parseInt(port));
		// Create input stream to receive data
		fromServer = new DataInputStream(socket.getInputStream());
		// Create output stream to send data
		toServer = new DataOutputStream(socket.getOutputStream());

		
    	initialOutput(outputFileTcp);
    	List<String[]> operations = csv.readCVS(inputFile);
    	for (int i = 1; i <operations.size(); i++) {
    		String operate = operations.get(i)[0];
    		String key = operations.get(i)[1];
    		String value = "";
    		if (operate.equals("PUT"))
    			value = operations.get(i)[2];
    		
    		// execute TCP
    		String json = createJsonPayload(operate, key, value);
    		Long requestTime = System.currentTimeMillis();
    		System.out.println(json);
    		
    		// protocol
    		toServer.writeInt(0);
    		toServer.writeInt(0);
    		toServer.writeInt(16 + json.length());
    		toServer.writeInt(0);
    		toServer.writeBytes(json);
    		toServer.flush();
    		
    		byte[] header = new byte[16];
    		fromServer.read(header, 0, 16);
    		
    		// where magic happens
    		int len = (int) ((((header[8] & 0xff) << 24) | ((header[9] & 0xff) << 16) | ((header[10] & 0xff) << 8) | ((header[11] & 0xff) << 0)));    
    		byte[] message = new byte[len];
    		fromServer.read(message, 0, len);
    		System.out.println(new String(message));

    		Long responseTime = System.currentTimeMillis();
    		
    		String duration = String.valueOf(responseTime - requestTime);
    		String[] record = {operate, key, value, String.valueOf(message), duration};
    		csv.appendCSV(outputFileTcp, record);
    	}
    	toServer.close();
    	fromServer.close();
    	socket.close();   	
    }
    
    
    
    // Initialize output file
    public static void initialOutput(String outputFile) {
    	if (csv.isCSVExists(outputFile)) {
			csv.clearCVS(outputFile);
		} else {
			csv.createCSV(outputFile);
		}
		String[] head = {"operation", "key", "value", "response", "duration"};
		csv.appendCSV(outputFile, head);
    }
    
    // Initialize json payload
    public static String createJsonPayload(String operation, String key, String value) {
		StringBuffer sb = new StringBuffer();
		sb.append("{\"jsonkv\":\"1.0\",\"operate\":\"").append(operation.toLowerCase())
				.append("\",\"key\":\"").append(key).append("\",\"value\":\"")
				.append(value).append("\",\"id\":\"1\"}");
		return sb.toString();
	}
}
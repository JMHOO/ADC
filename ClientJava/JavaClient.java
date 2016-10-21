/* TCSS558 Applied Distributed Computing 
 * 
 * Copyright (c) 2016 Yuyang Yu. */

import java.net.MalformedURLException;
import java.net.URL;  
import java.util.Scanner;
import com.googlecode.jsonrpc4j.JsonRpcHttpClient;  
import com.googlecode.jsonrpc4j.ProxyUtil;  
  
public class JavaClient {  
	
	private static String serverDomainName = "uw.umx.io";
	private static String serverIP ="73.140.72.152";
	private static String serverPortNumber = "15003";
	
	
    public static void main(String[] args) throws Throwable {  
    	String method = null;
    	// create a Scanner object
		Scanner input = new Scanner(System.in);
    	
		try {
    		if (args.length == 4) {
	    		//===========================
	    			
	    		//===========================
    		}
    		else if (args.length == 3) {
    			serverIP = args[0];
    			serverPortNumber= args[1];
    			method = args[2];
    		}
    		else if (args.length == 2) {
    			serverIP = args[0];
    			serverPortNumber= args[1];
    			// prompt the user to select protocol
    			System.out.println("Please enter TCP/UDP/RPC as communication protocol: ");
				method = input.nextLine();
    		}
    		else {
    			// prompt the user to enter the serverIP
				System.out.println("Please enter the server IP or name: ");		 /** Server name/IP 处理 */
				serverIP = input.nextLine();
				// prompt the user to enter the port number
				System.out.println("Please enter the server port number: ");
				serverPortNumber = input.nextLine();
				// prompt the user to select protocol
				System.out.println("Please enter TCP/UDP/RPC as communication protocol: ");
				method = input.nextLine();
    		}

    		// TCP/UDP/RPC
    		switch(method) {
	    		case "RPC":
	    			manulRPC();
	    			break;
	    		case "TCP":
	    			
	   				break;
	   			case "UDP":
	   				
	    			break;
	    		default:
	    			System.out.println("Wrong communication protocol command! Please enter TCP or UDP or RPC.");
	    			System.exit(0);
    		}
    	}
		catch(Exception ex) {
			ex.printStackTrace();
		}
		
		input.close();
    }
    
    // RPC
    private static void manulRPC() throws Throwable {
		// create a Scanner object
		Scanner input = new Scanner(System.in);
		// prompt the user to select command
		System.out.println("Please select your command: \n\t1 for PUT(key, value)\n\t2 for GET(key)\n\t3 for Delete(key)");
		int command = input.nextInt();
		input.nextLine();
		String key, value;
		Client c;
		// construct url
		String url = "http://".concat(serverIP).concat(":").concat(serverPortNumber);
		System.out.println("url: " + url);
		JsonRpcHttpClient client = new JsonRpcHttpClient(new URL(url));
		switch(command) {
			case 1:
				// prompt the user to enter key and value
				System.out.println("Please enter the key: ");
				key = input.nextLine();
				System.out.println("Please enter the value: ");
				value = input.nextLine();
				c = client.invoke("Put", new Object[] {key, value}, Client.class); 
		        System.out.println(c); 
				break;
			case 2:
				// prompt the user to enter key
				System.out.println("Please enter the key: ");
				key = input.nextLine();
				c = client.invoke("Get", new Object[] {key}, Client.class); 
		        System.out.println(c); 
				break;
			case 3:
				// prompt the user to enter key
				System.out.println("Please enter the key: ");
				key = input.nextLine();
				c = client.invoke("Delete", new Object[] {key}, Client.class); 
		        System.out.println(c); 
				break;
			default:
				System.out.println("Wrong command! Please follow the instruction!");
				System.exit(0);
				break;
		}
		// close the Scanner to avoid resource leak
		input.close();
	}
}  
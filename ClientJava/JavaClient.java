import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.List;

public class JavaClient {

	public static void main(String[] args) {
		
		Logger logger = LoggerFactory.getLogger(JavaClient.class);
		final String inputFile = "input.csv";
		
		ClientTcp myClientTcp = new ClientTcp();
		ClientUdp myClientUdp = new ClientUdp();
		String jsonPut = "{\"jsonkv\":\"1.0\",\"operate\":\"put\",\"key\":\"apple\",\"value\":\"www.apple.com\",\"id\":\"1\"}";
		String jsonGet = "{\"jsonkv\":\"1.0\",\"operate\":\"get\",\"key\":\"apple\",\"value\":null,\"id\":\"1\"}";
		String jsonDelete = "{\"jsonkv\":\"1.0\",\"operate\":\"delete\",\"key\":\"apple\",\"value\":\"www.apple.com\",\"id\":\"1\"}";

        long startTime = System.currentTimeMillis();
		myClientUdp.UdpTest();
		long endTime =System.currentTimeMillis();
		long duration = (endTime - startTime);
		System.out.println("   "+ duration + " milliseconds");
		// TODO Auto-generated method stub
	}
}

	
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.List;

public class JavaClient {
	private final String severIp;
	private final int port;
	private final Logger logger;
	private final OperateCSV csv;

	public JavaClient(String serverIp, int serverPort){
		this.severIp = serverIp;
		this.port = serverPort;
		this.logger = LoggerFactory.getLogger(JavaClient.class);
		this.csv = new OperateCSV();
	}

	public void initialOutPut(String outputFile) {
		if (csv.isCSVExists(outputFile)) {
			csv.clearCVS(outputFile);
		} else {
			csv.createCSV(outputFile);
		}
		String[] head = {"operation", "key", "value", "response", "duration"};
		csv.appendCSV(outputFile, head);
	}

	public void runOperations(String inputFile, String outputFile, String protocal) {
		try {
			initialOutPut(outputFile);
			List<String[]> operations = csv.readCVS(inputFile);
			for (int i = 1; i < operations.size(); i++) {
				String operate = operations.get(i)[0];
				String key = operations.get(i)[1];
				String value = "";
				if (operate.equals("PUT"))
					value = operations.get(i)[2];
				if (protocal.equals("TCP")) {
					executeTCP(outputFile, operate, key, value);
				} else if (protocal.equals("UDP")) {
					executeUDP(outputFile, operate, key, value);
				} else {
					System.out.println("wrong command");
				}

			}
		} catch (Exception e) {
			logger.info("exception while open input csv file");
		}
	}

	public String createJsonPayload(String operation, String key, String value) {
		StringBuffer sb = new StringBuffer();
		sb.append("{\"jsonkv\":\"1.0\",\"operate\":\"").append(operation.toLowerCase())
				.append("\",\"key\":\"").append(key).append("\",\"value\":\"")
				.append(value).append("\",\"id\":\"1\"}");
		return sb.toString();
	}

	public void executeUDP(String outputFile, String operation, String key, String value) {
		String json = createJsonPayload(operation, key, value);

		Long requestTime = System.currentTimeMillis();
		ClientUdp clientUdp = new ClientUdp(getIp(), getPort());
		String responseString =  clientUdp.UdpTest(json);
		Long responseTime = System.currentTimeMillis();

		String duration = String.valueOf(responseTime - requestTime);
		String[] record = {operation, key, value, String.valueOf(responseString), duration};
		csv.appendCSV(outputFile, record);
	}

	public void executeTCP(String outputFile, String operation, String key, String value) {
		String json = createJsonPayload(operation, key, value);

		Long requestTime = System.currentTimeMillis();
		ClientTcp clientTcp = new ClientTcp(getIp(), getPort());
		String responseString =  clientTcp.TcpTest(json);
		Long responseTime = System.currentTimeMillis();

		String duration = String.valueOf(responseTime - requestTime);
		String[] record = {operation, key, value, String.valueOf(responseString), duration};
		csv.appendCSV(outputFile, record);
	}

	public String getIp() {
		return this.severIp;
	}

	public int getPort() {
		return this.port;
	}


//		ClientTcp myClientTcp = new ClientTcp();
//		String jsonPut = "{\"jsonkv\":\"1.0\",\"operate\":\"put\",\"key\":\"apple\",\"value\":\"www.apple.com\",\"id\":\"1\"}";
//		String jsonGet = "{\"jsonkv\":\"1.0\",\"operate\":\"get\",\"key\":\"apple\",\"value\":null,\"id\":\"1\"}";
//		String jsonDelete = "{\"jsonkv\":\"1.0\",\"operate\":\"delete\",\"key\":\"apple\",\"value\":\"www.apple.com\",\"id\":\"1\"}";
		//myClientTcp.TcpTest(jsonPut);

}

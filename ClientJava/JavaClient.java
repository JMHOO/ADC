import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.List;

public class JavaClient {

	public static void main(String[] args) {
		
		Logger logger = LoggerFactory.getLogger(JavaClient.class);
		final String inputFile = "input.csv";
		
		OperateCSV csv = new OperateCSV();
		System.out.println(csv.isCSVExists(inputFile));
		try{
			List<String[]> operations = csv.readCVS(inputFile);
			for (int i=1; i < operations.size(); i++){
				String operate = operations.get(i)[0];
				String key = operations.get(i)[1];
				String value = operations.get(i)[2];
				
				System.out.println(operate);
			}
		}catch(Exception e){
			logger.info("exception while open input csv file");
		}
	
		
		ClientTcp myClientTcp = new ClientTcp();
		String jsonPut = "{\"jsonkv\":\"1.0\",\"operate\":\"put\",\"key\":\"apple\",\"value\":\"www.apple.com\",\"id\":\"1\"}";
		String jsonGet = "{\"jsonkv\":\"1.0\",\"operate\":\"get\",\"key\":\"apple\",\"value\":null,\"id\":\"1\"}";
		String jsonDelete = "{\"jsonkv\":\"1.0\",\"operate\":\"delete\",\"key\":\"apple\",\"value\":\"www.apple.com\",\"id\":\"1\"}";

       
	}
}

	
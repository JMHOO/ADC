import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.opencsv.CSVReader;
import com.opencsv.CSVWriter;

class OperateCSV {
	public boolean isCSVExists(String file_name) {
		return new File(file_name).exists();
	}

	public void createCSV(String file_name) {
		try {
			CSVWriter writer = new CSVWriter(new FileWriter(file_name, true));
			writer.close();
		} catch (IOException e) {
			System.out.println("Cannot create CSV file");
			e.printStackTrace();
		}
	}
	
	public void appendCSV(String file_name, String[] data) {
		try {
			CSVWriter csvWriter = new CSVWriter(new FileWriter(file_name, true));
			csvWriter.writeNext(data);
			csvWriter.close();
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void clearCVS(String file_name) {
		try {
			CSVWriter csvWriter = new CSVWriter(new FileWriter(file_name));
			csvWriter.flush();
			csvWriter.close();
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	
	public List<String[]> readCVS(String file_name) throws IOException {
		List<String[]> data = new ArrayList<String[]>();
		try {
			CSVReader reader = new CSVReader(new FileReader(file_name));
			data = reader.readAll();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return data;
	}

}


import java.util.Iterator;
import java.util.List;


/**
 * 
 */

/**
 * @author ronyronen
 * This calss assume ex1 has and execute method which receive input file name and output file name.
 * It loops on the sequence and compare the results between outputs and tests files.
 * i.e: 
 *   ex1 instance = new ex1();
 *	 instance.execute(inputFileName, outputFileName);
 */

public class CompareFiles {

	final static String INPUT_FILE_NAME_1 = "input"; 
	final static String OUTPUT_FILE_NAME_1 = "output";
	final static String TEST_FILE_NAME = "test"; //test files prefix
	final static String OUTPUT_FILE_NAME = "results";
	final static String SUFFIX_FILE_NAME = "txt";
	
	final static int SEQUENCE_ST = 1;
	final static int SEQUENCE_END = 7;
	
	public static void main(String[] args) {
		try {
			CompareFiles instance = new CompareFiles();
			instance.execute();
		} catch (Exception e) {
			Utils.log(e);
			e.printStackTrace();
		}
	}
	
	
	public void execute () {
		try {
			String inputFileName = null;
			String outputFileName = null;
			String compareFileName = null;
			//String resultFileName = null;
			
			for (int i = SEQUENCE_ST ; i <= SEQUENCE_END; i++) {
				inputFileName = INPUT_FILE_NAME_1 + i + "." + SUFFIX_FILE_NAME;
				outputFileName = OUTPUT_FILE_NAME_1 + i + "." + SUFFIX_FILE_NAME;
				
				ex1 instance = new ex1();
				instance.execute(inputFileName, outputFileName);
			}
			
			Utils.log("*******");
			int diffCounter = 0;
			for (int i = SEQUENCE_ST ; i <= SEQUENCE_END; i++) {
				inputFileName = INPUT_FILE_NAME_1 + i + "." + SUFFIX_FILE_NAME;
				outputFileName = OUTPUT_FILE_NAME_1 + i + "." + SUFFIX_FILE_NAME;
				compareFileName = TEST_FILE_NAME + i + "." + SUFFIX_FILE_NAME;
				
				List <String> file1 = Utils.readSmallTextFile(outputFileName);
				List <String> file2 = Utils.readSmallTextFile(compareFileName);
				
				Iterator <String>iter1 = file1.iterator();
				Iterator <String> iter2 = file2.iterator();
				
				int lineNumber = 0;
				
				while (iter2.hasNext()) {
					lineNumber++;
					String line2 = iter2.next();
					String line1 = iter1.next();
					if(!line2.equals(line1)){
						Utils.log(">>>>>>>>>>" + " File: " + inputFileName + ">>>>>>>>>>");
						Utils.log("Expected: " + line2);
						Utils.log("Actual:   " + line1 + " at line: " + lineNumber);
						diffCounter++;
					}
				}
			}
			Utils.log("======" + " Error Diff: " + diffCounter + " ======");
			
		} catch (Exception e) {
			Utils.log(e);
			e.printStackTrace();
		} 
	}
}

package cn.zeki.flat;

import java.util.Random;

public class Util {
	
	public static String getClassName()
	{
		  return Thread.currentThread() .getStackTrace()[2].getClassName();
	}
	
	public static String getMethodName()
	{
		return Thread.currentThread() .getStackTrace()[2].getMethodName();
	}
	
	public static String extract(String string, String startString, String endString)
	{
		int start = string.indexOf(startString);
		if (start < 0) {
			return "";
		}
		start += startString.length();
		int end = string.indexOf(endString, start);
		if (end < 0) {
			return "";
		}
		
		String children = string.substring(start, end);
		return children;
	}
	
	public static String getRandomString(int length)
	{
		String val = "";
		Random random = new Random();
        for(int i = 0; i < length; i++)     
        {     
            String charOrNum = random.nextInt(2) % 2 == 0 ? "char" : "num"; // 输出字母还是数字
                      
            if("char".equalsIgnoreCase(charOrNum)) // 字符串     
            {     
                int choice = random.nextInt(2) % 2 == 0 ? 65 : 97; //取得大写字母还是小写字母     
                val += (char) (choice + random.nextInt(26));     
            }     
            else if("num".equalsIgnoreCase(charOrNum)) // 数字     
            {     
                val += String.valueOf(random.nextInt(10));
            }     
        }
        //val=val.toLowerCase();
        return val;
	}
}

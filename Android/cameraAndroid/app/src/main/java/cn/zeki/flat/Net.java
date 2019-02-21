package cn.zeki.flat;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.util.EntityUtils;

import java.io.IOException;

public class Net {
	public static String get(String strUrl)
	{
		String strResult="";
		
		//创建httpRequest对象 
		HttpGet httpRequest = new HttpGet(strUrl);
		try
		{
			//取得HttpClient对象
			HttpClient httpclient = new DefaultHttpClient();
			//请求HttpClient，取得HttpResponse
			HttpResponse httpResponse = httpclient.execute(httpRequest);
			//请求成功
			if (httpResponse.getStatusLine().getStatusCode() == HttpStatus.SC_OK)
			{
				//取得返回的字符串
				strResult = EntityUtils.toString(httpResponse.getEntity());
			}
		}
		catch (Exception e) {
			// TODO: handle exception
		}
		
		return strResult;
	}
	
	public static String post(String url, HttpEntity entity)
	{
		HttpPost httpPost = new HttpPost(url);
		BasicHttpParams httpParams = new BasicHttpParams();

		HttpConnectionParams.setConnectionTimeout(httpParams, 2000);
		HttpConnectionParams.setSoTimeout(httpParams, 3000);

		HttpClient httpClient = new DefaultHttpClient(httpParams);
		String strResult="";
	    try {
	        httpPost.setEntity(entity);
	        HttpResponse httpResponse = httpClient.execute(httpPost);
	        if (httpResponse.getStatusLine().getStatusCode() == HttpStatus.SC_OK)
			{
				// 取得返回的数据
				strResult = EntityUtils.toString(httpResponse.getEntity());
			}
	    } catch (IOException e) {
	        e.printStackTrace();
	    }
	    finally {
			httpClient.getConnectionManager().shutdown();
		}

	    return strResult;
	}
}

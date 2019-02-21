package cn.zeki.flat;

import org.apache.http.HttpEntity;

import cn.zeki.flat.Flat.Run;

public class Block {
	public static String get(String url)
	{
		Flat.put("getUrl", url);
		Flat.run(Flat.BACK, new Run() {
			
			@Override
			public void run() {
				String ret = Net.get(Flat.getString("getUrl"));
				Flat.put("getRet", ret);
			}
		});
		
		return Flat.getString("getRet");
	}
	
	public static String post(String url, HttpEntity entity)
	{
		Flat.put("postUrl", url);
		Flat.put("postEntity", entity);
		Flat.run(Flat.BACK, new Run() {
			
			@Override
			public void run() {
				String ret = Net.post(Flat.getString("postUrl"),(HttpEntity)Flat.get("postEntity"));
				Flat.put("postRet", ret);
			}
		});
		
		return Flat.getString("postRet");
	}
}

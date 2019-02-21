package cn.zeki.flat;

import cn.zeki.flat.Flat.Run;


class RunWrap implements Runnable
{
	Run run;
	int type;

	public RunWrap(Run run,int type)
	{
		this.run = run;
		this.type = type;
	}
	
	@Override
	public void run() {
		run.run();
	}
}
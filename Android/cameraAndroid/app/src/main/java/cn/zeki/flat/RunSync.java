package cn.zeki.flat;

import java.util.concurrent.Semaphore;

import cn.zeki.flat.Flat.Run;


class RunSync implements Runnable
{
	Run run;
	int type;
	Semaphore signal;
	
	public RunSync(Run run,int type,Semaphore signal)
	{
		this.run = run;
		this.type = type;
		this.signal = signal;
	}
	
	@Override
	public void run() {
		run.run();
		signal.release();
	}
}
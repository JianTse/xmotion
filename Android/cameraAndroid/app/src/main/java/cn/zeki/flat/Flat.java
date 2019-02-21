package cn.zeki.flat;

import android.os.Handler;
import android.os.Looper;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.Semaphore;

public class Flat {
	
	public static final int UI   = 0;  // UI task
	public static final int BACK = 1;  // Background task
	
	public interface Run
	{
		public void run();
	}

	private static HashMap<String, Object> bundle = new HashMap<>();
	// UI handler
	private static Handler uiHandler = new Handler(Looper.getMainLooper());
	// Background thread
	private static Thread mainThread;
	// Locks
	private static Semaphore taskRunLock = new Semaphore(1);
	private static Semaphore taskListLock = new Semaphore(0);
	// Stop flag
	private static boolean  stop = false;
	
	protected class Task {
		int type = BACK;
		Run      run;    
		public Task(Run run,int type)
		{
			this.run = run;
			this.type = type;
		}
		
		public void doRun()
		{
			if (type == BACK) {
				run.run();
				taskRunLock.release();
			}
			else {
				uiHandler.post(new Runnable() {
					
					@Override
					public void run() {
						run.run();
						taskRunLock.release();
					}
				});
			}
		}
	}

	// Task queue
	private static List<Task> taskList = new ArrayList<>();

	// Instance
	private static Flat mInstance;

	private Flat()
	{
		mainThread = new Thread(new Runnable() {
			
			@Override
			public void run() {
				loop();
			}
		});
		
	}

	private static void start()
	{
		getInstance();
		
		if (mainThread.isAlive()) {
			return ;
		}
		
		mainThread.start();
	}
	
	private void loop()
	{
		while (!stop) {
			// Wait last task done
			try {
				taskRunLock.acquire();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			
			if (stop) {
				break;
			}
			
			// acquire a new task
			Task task = getTask();
			
			if (stop || task == null) {
				break;
			}
			
			// do the task
			task.doRun();
			// remove from task queue
			popTask();
		}
	}

	private Task getTask()
	{
		while(true)
		{
			synchronized(taskList) {
				if (taskList.size() > 0) {
					return taskList.get(0);
				}
			}
			
			if (stop) {
				break;
			}
			
			// wait for the task queue
			try {
				taskListLock.acquire();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		return null;
	}

	private void addTask(Run run,int type)
	{
		Task task = new Task(run,type);
		synchronized(taskList) {
			taskList.add(task);
			taskListLock.release();
		}
	}
	
	private void popTask()
	{
		synchronized(taskList) {
			if (taskList.size() > 0) {
				taskList.remove(0);
			}
		}
	}

	private static Flat getInstance() {
		if (mInstance == null) {
			synchronized (Flat.class) {
				if (mInstance == null) {
					mInstance = new Flat();
				}
			}
		}
		return mInstance;
	}

	/**
	 * Queue a task to schedule list. 
	 * @param type
	 * @param run
	 */
	public static void queue(int type,Run run) 
	{
		if (isBackThread() || BACK == type) {
			// do it now
			run.run();
			return ;
		}
		
		getInstance().addTask(run,type);
		// automatically start
		start();
	}
	
	public static boolean isBackThread()
	{
		return Thread.currentThread() == mainThread;
	}
	
	public static boolean isUiThread()
	{
		return Looper.getMainLooper() == Looper.myLooper();
	}
	
	/**
	 * Run the task now and wait until it's done.
	 * @param type
	 * @param run
	 */
	public static void run( int type, Run run)
	{
		// if we ere in the back thread
		if (isBackThread() && BACK == type) {
			run.run();
			return ;
		}
		
		if (isUiThread() && UI == type) {
			run.run();
			return ;
		}
		
		// start the job now
		Semaphore signal = new Semaphore(0);
		RunSync wrap = new RunSync(run, type, signal);
		if (type == BACK) {
			new Thread(wrap).start();
		}
		else {
			uiHandler.post(wrap);
		}
		
		// wait for completion
		try {
			signal.acquire();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Start a task and return
	 * @param type
	 * @param run
	 */
	public static void post(int type,Run run)
	{
		// start the job now
		RunWrap wrap = new RunWrap(run, type);
		if (type == BACK) {
			new Thread(wrap).start();
		}
		else {
			uiHandler.post(wrap);
		}
	}
	
	/**
	 * Cancel the task queue
	 */
	public static void cancel()
	{
		synchronized(taskList) {
			taskList.clear();
			taskListLock.release();
		}
	}
	
	public static void sleep(int ms)
	{
		try {
			Thread.sleep(ms);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Stop Flat background thread
	 */
	public static void stop()
	{
		stop = true;
		Flat.taskRunLock.release();
	}
	
	/**
	 * Store a key-value pair.
	 * @param key
	 * @param value
	 */
	public static void put(String key, Object value)
	{
		bundle.put(key, value);
	}
	
	/**
	 * Get value for a key from the store.
	 * @param key
	 * @return value for the key
	 */
	public static Object get(String key)
	{
		return bundle.get(key);
	}
	public static boolean getBoolean(String key)
	{
		Object object = get(key);
		if (object == null) {
			return false;
		}
		
		return (boolean)object;
	}
	public static int getInt(String key)
	{
		Object object = get(key);
		if (object == null) {
			return 0;
		}
		
		return (int)object;
	}
	public static long getLong(String key)
	{
		Object object = get(key);
		if (object == null) {
			return 0;
		}
		
		return (long)object;
	}
	public static float getFloat(String key)
	{
		Object object = get(key);
		if (object == null) {
			return 0;
		}
		
		return (float)object;
	}
	public static double getDouble(String key)
	{
		Object object = get(key);
		if (object == null) {
			return 0;
		}
		
		return (double)object;
	}
	
	public static String getString(String key)
	{
		Object object = get(key);
		if (object == null) {
			return "";
		}
		
		return (String)object;
	}
}

import com.pi4j.io.gpio.*;
import java.util.*;
import java.text.*;
import java.lang.*;

public class ControlSegment implements Runnable{

    private String parameter;
    private List<Pin> segmentsPins, digitsPin;
    private List<GpioPinDigitalOutput> segments, digits;
    private GpioController gpio;
    Map<String, Integer[]> numbers;
    
    public ControlSegment(String parameter) {
	this.parameter = parameter;
    }

    public void OffSegment(){
	this.digits.get(0).high();
	this.digits.get(1).high();
	this.digits.get(2).high();
	this.digits.get(3).high();
	this.parameter=new String();
    }

    public void setDigit(String digit) {
	this.parameter = digit;
    }

    public void segmentInit() {
	System.out.println("Segment Testing...");

	int i, j, k;

	/*final GpioController */gpio = GpioFactory.getInstance();

	/*List<Pin> */this.segmentsPins = new ArrayList(Arrays.asList(RaspiPin.GPIO_00,
							    RaspiPin.GPIO_22,
							    RaspiPin.GPIO_24,
							    RaspiPin.GPIO_02,
							    RaspiPin.GPIO_25,
							    RaspiPin.GPIO_21,
							    RaspiPin.GPIO_23,
							    RaspiPin.GPIO_07));
	/*List<GpioPinDigitalOutput> */this.segments = new ArrayList<>();

	for (i=0; i<this.segmentsPins.size(); i++) {
	    this.segments.add(this.gpio.provisionDigitalOutputPin(this.segmentsPins.get(i), PinState.LOW));
	}
	
	/*List<Pin> */this.digitsPin = new ArrayList(Arrays.asList(RaspiPin.GPIO_26,
							  RaspiPin.GPIO_05,
							  RaspiPin.GPIO_03,
							  RaspiPin.GPIO_01));
	/*List<GpioPinDigitalOutput> */this.digits = new ArrayList<>();

	for(i=0; i<this.digitsPin.size(); i++) {
	    this.digits.add(this.gpio.provisionDigitalOutputPin(this.digitsPin.get(i), PinState.HIGH));
	}

        System.out.println(this.parameter.charAt(this.parameter.length()-1));

	this.numbers = new HashMap();
	this.numbers.put("0", new Integer[]{1,1,1,1,1,1,0});
	this.numbers.put("1", new Integer[]{0,1,1,0,0,0,0});
	this.numbers.put("2", new Integer[]{1,1,0,1,1,0,1});
	this.numbers.put("3", new Integer[]{1,1,1,1,0,0,1});
	this.numbers.put("4", new Integer[]{0,1,1,0,0,1,1});
	this.numbers.put("5", new Integer[]{1,0,1,1,0,1,1});
	this.numbers.put("6", new Integer[]{1,0,1,1,1,1,1});
	this.numbers.put("7", new Integer[]{1,1,1,0,0,0,0});
	this.numbers.put("8", new Integer[]{1,1,1,1,1,1,1});
	this.numbers.put("9", new Integer[]{1,1,1,1,0,1,1});
	this.numbers.put("C", new Integer[]{1,0,0,1,1,1,0});

	System.out.println(numbers.get("1")[3]);
    }
    
    public void run() {//main(String[] args) throws InterruptedException {
	int i, j, k;

	System.out.println("digit: " + parameter + ", len: " + parameter.length());
	
	try{
	    while(true) {
		for(i=0; i<3; i++) { //digits.size() = 4
		    for(j=0; j<segments.size()-1; j++) { //segments.size()-1 = 7
			segments.get(j).low();
			if(numbers.get(String.valueOf(parameter.charAt(parameter.length()-i-1)))[j]==1)
			    segments.get(j).high();
			else
			    segments.get(j).low();
			if(i==1)
			    segments.get(7).high();
			else
			    segments.get(7).low();
		    }
		    for(k=0; k<3; k++) {
			if(k==i)
			    continue;
			digits.get(k).high();
		    }
		    digits.get(i).low();
		    Thread.sleep(5);
		}
	    }
	} catch(Exception e) {
	    e.printStackTrace();
	}
	
    }
}

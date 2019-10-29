CC = gcc
JNI1 = /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/include/
JNI2 = /usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/include/linux/
SSL1 = /usr/local/lib/
SSL2 = /usr/local/include/
PI4J = /opt/pi4j/lib/'*'
JAVA = javac

libIBSModule2.so : IBSClient.o
	$(CC) -shared -o libIBSModule2.so IBSClient.o libcrypto.so libssl.so
IBSClient.o : IBSClient.c
	$(CC) -I$(JNI1) -I$(JNI2) -I$(SSL1) -I$(SSL2) -fPIC -lssl -lcrypto -c IBSClient.c
ControlSegment.class : ControlSegment.java
	$(JAVA) -classpath .:classes:$(PI4J) -d . ControlSegment.java
IBSClient.class : IBSClient.java HelperMethods.java ControlSegment.java
	$(JAVA) -classpath .:classes:$(PI4J) -d . IBSClient.java HelperMethods.java ControlSegment.java
clean :
	rm -fv libIBSModule2.so IBSClient.o ControlSegment.class IBSClient.class

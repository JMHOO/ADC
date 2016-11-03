JFLAGS = -classpath .:opencsv-3.7.jar:slf4j-api-1.7.21.jar
JC = javac
JVM= java

.SUFFIXES: .java .class
.java.class:
		$(JC) $(JFLAGS) $*.java

CLASSES = \
    OperateCSV.java \
	JavaMain.java
MAIN = JavaMain

# default: classes
default:
	@echo "make: compile .java."
	@echo "make clean: remove .class files."
	@echo "make run: run your app."

classes: $(CLASSES:.java=.class)

run: $(MAIN).class
		$(JVM) $(MAIN)

clean:
		$(RM) *.class
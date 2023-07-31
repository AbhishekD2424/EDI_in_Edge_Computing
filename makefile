# Run the 'make' command from your terminal only if you are running the service vendor and all the edge servers on the same computer.
# Run 'make sv' if you want to compile the service vendor only. Similarly, 'make es1' for edge server 1...
#Run 'make clean' to delete all the object files
# If service vendor and edge servers are running in different computers then copy the following commands and run in the terminals of the appropriate compuetrs.

all: sv es1 es2

sv:
	@echo "Compiling the Sevice Vendor..."
	g++ -I/usr/include/botan-2 -L/usr/lib -Wl,-rpath=/usr/lib -Wall -o sv service_vendor.cpp -lbotan-2

es1:
	@echo "Compiling the Edge Server 1..."
	g++ -I/usr/include/botan-2 -L/usr/lib -Wl,-rpath=/usr/lib -Wall -o es1 edge_server_1.cpp -lbotan-2

es2:
	@echo "Compiling the Edge Server 2..."
	g++ -I/usr/include/botan-2 -L/usr/lib -Wl,-rpath=/usr/lib -Wall -o es2 edge_server_2.cpp -lbotan-2

clean:
	@echo "Removing all object files..."	
	rm sv es1 es2


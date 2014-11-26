all:
	$(MAKE) -C src all

server:
	$(MAKE) -C src server
	
client: 
	$(MAKE) -C src client

clean:
	$(MAKE) -C src clean

clean-all:
	$(MAKE) -C src clean-all

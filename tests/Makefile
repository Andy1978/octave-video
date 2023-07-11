.PHONY: run realclean

run:
	make -C .. dist
	./build.sh
	./run.sh | tee docker_results.log

realclean:
	rm -f docker_results.log

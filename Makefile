all:
	@./do
clean:
	rm -f tags test lib.so inject.so
	rm -f test.tmp/lib.so
	[ -d test.tmp ] && rmdir test.tmp || true

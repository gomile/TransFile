DIRS = transFileCli transFileSrv

all:
	@for subdir in $(DIRS); \
	do \
	(cd $$subdir && $(MAKE) || exit 1); \
	done;

clean:
	@for subdir in $(DIRS); \
	do \
	(cd $$subdir && $(MAKE) clean || exit 1); \
	done;

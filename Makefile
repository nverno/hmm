TOP := $(patsubst %/,%,$(abspath $(dir $(lastword $(MAKEFILE_LIST)))))

SRC        = $(wildcard $(TOP)/src/*.rs)
BUILD_DIR  = $(TOP)/target/debug
TARGET     = $(BUILD_DIR)/libtree_query.so
LIB        = $(patsubst %libtree_query.so,%tree-query.so, $(TARGET))

all:
	@


$(TARGET): $(SRC)
	@cargo build >/dev/null 2>&1

$(LIB): $(TARGET)
	@ln -s $(TARGET) $(LIB)

build: $(LIB)

emacs: build ## Launch emacs with library loaded
	@emacs -Q -L $(BUILD_DIR) \
		--eval "(require 'tree-query)"

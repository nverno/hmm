TOP := $(patsubst %/,%,$(abspath $(dir $(lastword $(MAKEFILE_LIST)))))

SRC       := $(wildcard $(TOP)/src/*.cc)
TARGET    ?= target
RS_DIR     = $(TOP)/tsq
RS_SRC     = $(wildcard $(RS_DIR)/src/*.rs)
RS_TARGET  = $(RS_DIR)/target/debug/libtsq.so
RS_LIB     = $(patsubst %libtsq.so,%tsq.so, $(RS_TARGET))
LIB        = $(TARGET)/tsmeta.so
FEATURE   ?= tsmeta
LOADPATH  ?= $(TARGET)

all:
	@

tsmeta: $(LIB)
$(LIB): $(SRC)
	$(RM) $@
	mkdir -p target
	$(CXX) -o $@ -Isrc $^ -shared -fPIC -Os

emacs: tsmeta
	@emacs -Q -L $(LOADPATH) --eval "(require '$(FEATURE))"


$(RS_TARGET): $(RS_SRC)
	@cd $(RS_DIR) && cargo build >/dev/null 2>&1

$(RS_LIB): $(RS_TARGET)
	@ln -s $(RS_TARGET) $(RS_LIB) || true
tsq: $(RS_LIB)

# emacs: build ## Launch emacs with library loaded
# 	@emacs -Q -L $(RUST_BUILD) \
# 		--eval "(require 'tsq)"

TOP := $(patsubst %/,%,$(abspath $(dir $(lastword $(MAKEFILE_LIST)))))

SRC       := $(wildcard $(TOP)/src/*.cc)
TARGET    ?= build/lib
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
	@cmake -B build && make -C build
# $(RM) $@
# mkdir -p target
# $(CXX) -o $@ -Isrc $^ -shared -fPIC -Os -lstdc++fs
# -I/home/noah/src/emacs/src/ -I/home/noah/src/emacs/lib

.PHONY: emacs
emacs: tsmeta  ## Launch emacs, require shared lib and run ielm
	@emacs -Q -L $(LOADPATH) --eval "(require '$(FEATURE))" --eval "(ielm)"


$(RS_TARGET): $(RS_SRC)
	@cd $(RS_DIR) && cargo build >/dev/null 2>&1

$(RS_LIB): $(RS_TARGET)
	@ln -s $(RS_TARGET) $(RS_LIB) || true
tsq: $(RS_LIB)

# emacs: build ## Launch emacs with library loaded
# 	@emacs -Q -L $(RUST_BUILD) \
# 		--eval "(require 'tsq)"

.PHONY: help
help:  ## Show help for targets
	@grep -E '^[/.%0-9a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) |     \
	sort | awk                                                      \
	'BEGIN {FS = ":[^:#]*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

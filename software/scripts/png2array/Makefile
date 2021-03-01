################################################################################
# Makefile for go project
################################################################################
TGT := png2array
SRC := png2array.go
GOPKGS    := \
	github.com/google/gopacket \
	github.com/google/gopacket/layers \
	github.com/google/gopacket/pcapgo \
	golang.org/x/net/bpf

# if no GOPATH set, use "~/go" as default
ifeq ($(GOPATH),)
GOPATH := ~/go
endif

default : test

################################################################################
# go get rule generator
define GOGET_FOREACH
PREFIXED_GOPKGS := $(PREFIXED_GOPKGS) $(GOPATH)/src/$(1)
$(GOPATH)/src/$(1) :
	go get -u $(1)
endef
$(foreach P, $(GOPKGS), $(eval $(call GOGET_FOREACH,$(P))))
################################################################################

test : $(TGT)
	./$(TGT) -m rgb565 testcard_1280x60.png out.h

$(TGT) : $(SRC) $(PREFIXED_GOPKGS)
	go fmt $(firstword $^)
	go build $(firstword $^)
	chmod +x $@

clean:
	rm -f $(TGT)

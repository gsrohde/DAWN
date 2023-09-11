extra_sample_output_directory = output-extra

INPUTS = $(wildcard sample_input/degenerate-biocro-systems/*.xml)

OUTPUTS = $(patsubst sample_input/degenerate-biocro-systems/%.xml,$(extra_sample_output_directory)/%-output.xml,$(INPUTS))

.PHONY: run_extra_samples

run_extra_samples: $(OUTPUTS)

$(OUTPUTS): main $(wildcard xml_schema/*.xsd)

$(OUTPUTS): $(extra_sample_output_directory)/%-output.xml : sample_input/degenerate-biocro-systems/%.xml
	mkdir -p $(extra_sample_output_directory)
	./main -wfile=$@ $<

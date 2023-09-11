.PHONY: run_samples 1 2 3 4 5 6 7

run_samples: 1 2 3 4 5 6 7

1: output/harmonic_oscillator_simulation.xml
2: output/miscanthus_2002_simulation.xml
3: output/willow_2002_simulation.xml
4: output/willow_2005_simulation.xml
5: output/miscanthus_2002_simulation-external_drivers.xml
6: output/willow_2002_simulation-external_drivers.xml
7: output/willow_2005_simulation-external_drivers.xml

output/harmonic_oscillator_simulation.xml \
    output/miscanthus_2002_simulation.xml \
    output/willow_2002_simulation.xml \
    output/willow_2005_simulation.xml \
    output/miscanthus_2002_simulation-external_drivers.xml \
    output/willow_2002_simulation-external_drivers.xml \
    output/willow_2005_simulation-external_drivers.xml: \
        main $(wildcard xml_schema/*.xsd)


output/harmonic_oscillator_simulation.xml: sample_input/harmonic_oscillator_system.xml
	mkdir -p output
	./main -wfile=output/harmonic_oscillator_simulation.xml sample_input/harmonic_oscillator_system.xml


output/miscanthus_2002_simulation.xml: sample_input/biocro-system.miscanthus.2002.xml
	mkdir -p output
	./main -wfile=output/miscanthus_2002_simulation.xml sample_input/biocro-system.miscanthus.2002.xml


output/willow_2002_simulation.xml: sample_input/biocro-system.willow.2002.xml
	mkdir -p output
	./main -wfile=output/willow_2002_simulation.xml sample_input/biocro-system.willow.2002.xml



output/willow_2005_simulation.xml: sample_input/biocro-system.willow.2005.xml
	mkdir -p output
	./main -wfile=output/willow_2005_simulation.xml sample_input/biocro-system.willow.2005.xml



output/miscanthus_2002_simulation-external_drivers.xml: sample_input/biocro-system.miscanthus.xml sample_input/2002_weather.xml
	mkdir -p output
	./main -wfile=output/miscanthus_2002_simulation-external_drivers.xml \
		-driversfile=sample_input/2002_weather.xml sample_input/biocro-system.miscanthus.xml


output/willow_2002_simulation-external_drivers.xml: sample_input/biocro-system.willow.xml sample_input/2002_weather.xml
	mkdir -p output
	./main -wfile=output/willow_2002_simulation-external_drivers.xml \
		-driversfile=sample_input/2002_weather.xml sample_input/biocro-system.willow.xml


output/willow_2005_simulation-external_drivers.xml: sample_input/biocro-system.willow.xml sample_input/2005_weather.xml
	mkdir -p output
	./main -wfile=output/willow_2005_simulation-external_drivers.xml \
		-driversfile=sample_input/2005_weather.xml sample_input/biocro-system.willow.xml

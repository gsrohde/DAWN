#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <StrX.h>
#include <xstr.h> // includes XX() macro

#include "run_app.h"

using namespace std;
using namespace xercesc;


void compare_last_row_with_expected(const vector<string>& command_line,
                                    const vector<double>& expected_value)
{
    auto result = run_app(command_line);

    DOMNodeList* row_list = result->get_elements_by_tag_name("row");
    DOMElement* last_item = dynamic_cast< DOMElement* >(row_list->item(row_list->getLength() - 1));
    DOMNodeList* variable_list = last_item->getElementsByTagName(XX("variable"));

    for (int i = 0; i < variable_list->getLength(); ++i) {
        DOMElement* variable = dynamic_cast< DOMElement* >(variable_list->item(i));
        const XMLCh* name = variable->getAttribute(XX("name"));
        const XMLCh* value = variable->getAttribute(XX("value"));
        // cout << StrX(name) << ": " << StrX(value) << endl;

        EXPECT_DOUBLE_EQ(std::stod(string(StrX(value).localForm())), expected_value[i]);
    }
}


TEST(RegressionTests, TestHarmonicOscillator) {
    vector<string> command_line {"", "../app/sample_input/harmonic_oscillator_system.xml"};

    // The EXPECT tests called in the compare_last_row_with_expected
    // function called below make use of the fact that
    // getElementsByTagName returns matching elements in document
    // order, which happens to be alphabetical order.
    vector<double> expected_value {
        6.28, // elapsed_time
        0.000507307, // kinetic_energy
        9.99995, // position
        49.9995, // spring_energy
        50, // total_energy
        0.031853 // velocity
    };

    compare_last_row_with_expected(command_line, expected_value);
}


TEST(RegressionTests, TestMiscanthus2002) {
    vector<string> command_line {"", "-d", "../app/sample_input/2002_weather.xml",
                                 "../app/sample_input/biocro-system.miscanthus.xml"};

    // The EXPECT tests called in the compare_last_row_with_expected
    // function called below make use of the fact that
    // getElementsByTagName returns matching elements in document
    // order, which happens to be alphabetical order.
    vector<double> expected_value {
        0, // Grain
        0, // GrossAssim
        2.06262, // Leaf
        0.292759, // LeafLitter
        0.389492, // LeafWS
        11.1541, // Rhizome
        0, // RhizomeLitter
        2.87031, // Root
        0, // RootLitter
        0, // Shell
        1.7, // Sp
        30.0035, // Stem
        0.639085, // StemLitter
        0.760048, // StomataWS
        3867.71, // TTc
        0.04, // alpha
        -0.000775326, // canopy_assimilation_rate
        214.048, // canopy_conductance
        0.250165, // canopy_transpiration_rate
        -0.84183, // cosine_zenith_angle
        0.204823, // cws1
        0.32, // cws2
        287, // doy
        6.57081, // gmst
        23, // hour
        2.45256e+06, // julian_date
        0, // kGrain
        0.01, // kLeaf
        0.35, // kRhizome
        0.01, // kRoot
        0.63, // kStem
        3.50646, // lai
        1088, // leaf_senescence_index
        170.531, // lha
        0.704144, // lmst
        0, // net_assimilation_rate_grain
        0, // net_assimilation_rate_leaf
        0, // net_assimilation_rate_rhizome
        0, // net_assimilation_rate_root
        0, // net_assimilation_rate_stem
        0, // precip
        0.62275, // rh
        1869, // rhizome_senescence_index
        0, // root_senescence_index
        0.697322, // soil_evaporation_rate
        0.291206, // soil_water_content
        0, // solar
        203.562, // solar_L
        342.452, // solar_azimuth_angle
        -8.44683, // solar_dec
        201.672, // solar_ell
        23.4386, // solar_ep
        280.582, // solar_g
        -159.969, // solar_ra
        147.334, // solar_zenith_angle
        542, // stem_senescence_index
        5.635, // temp
        287.958, // time
        -6, // time_zone_offset
        39, // vmax
        2.575, // windspeed
        2002 // year
    };

    compare_last_row_with_expected(command_line, expected_value);
}


TEST(RegressionTests, TestMiscanthus2005) {
    vector<string> command_line {"", "-d", "../app/sample_input/2005_weather.xml",
                                 "../app/sample_input/biocro-system.miscanthus.xml"};

    // The EXPECT tests called in the compare_last_row_with_expected
    // function called below make use of the fact that
    // getElementsByTagName returns matching elements in document
    // order, which happens to be alphabetical order.
    vector<double> expected_value {
        0, // Grain
        9.83485e-06, // GrossAssim
        1.67089, // Leaf
        0.324997, // LeafLitter
        0.389335, // LeafWS
        10.9034, // Rhizome
        0, // RhizomeLitter
        2.66448, // Root
        0, // RootLitter
        0, // Shell
        1.7, // Sp
        27.9671, // Stem
        0.522391, // StemLitter
        0.759949, // StomataWS
        3681.38, // TTc
        0.04, // alpha
        -0.000604756, // canopy_assimilation_rate
        173.373, // canopy_conductance
        0.118093, // canopy_transpiration_rate
        -0.885732, // cosine_zenith_angle
        0.204776, // cws1
        0.32, // cws2
        301, // doy
        7.50872, // gmst
        23, // hour
        2.45367e+06, // julian_date
        0, // kGrain
        0.01, // kLeaf
        0.35, // kRhizome
        0.01, // kRoot
        0.63, // kStem
        2.84051, // lai
        1060, // leaf_senescence_index
        171.066, // lha
        1.64205, // lmst
        0, // net_assimilation_rate_grain
        0, // net_assimilation_rate_leaf
        0, // net_assimilation_rate_rhizome
        0, // net_assimilation_rate_root
        0, // net_assimilation_rate_stem
        0, // precip
        0.75365, // rh
        1525, // rhizome_senescence_index
        0, // root_senescence_index
        1.08769, // soil_evaporation_rate
        0.291194, // soil_water_content
        0.092, // solar
        217.63, // solar_L
        341.014, // solar_azimuth_angle
        -13.4789, // solar_dec
        215.874, // solar_ell
        23.4381, // solar_ep
        294.598, // solar_g
        -146.435, // solar_ra
        152.342, // solar_zenith_angle
        390, // stem_senescence_index
        5.175, // temp
        301.958, // time
        -6, // time_zone_offset
        39, // vmax
        2.66, // windspeed
        2005 // year
    };

    compare_last_row_with_expected(command_line, expected_value);
}


TEST(RegressionTests, TestWillow2005) {
    vector<string> command_line {"", "-d", "../app/sample_input/2005_weather.xml",
                                 "../app/sample_input/biocro-system.willow.xml"};

    // The EXPECT tests called in the compare_last_row_with_expected
    // function called below make use of the fact that
    // getElementsByTagName returns matching elements in document
    // order, which happens to be alphabetical order.
    vector<double> expected_value {
        0, // Grain
        5.05994e-07, // GrossAssim
        0.00578966, // Leaf
        0.404188, // LeafLitter
        0.821445, // LeafWS
        1.46608, // Rhizome
        0, // RhizomeLitter
        1.5961, // Root
        0, // RootLitter
        0, // Shell
        1.1, // Sp
        5.14999, // Stem
        0, // StemLitter
        0.75023, // StomataWS
        3681.38, // TTc
        0, // alpha
        -1.01644e-06, // canopy_assimilation_rate
        0.000224362, // canopy_transpiration_rate
        -0.885732, // cosine_zenith_angle
        0.20011, // cws1
        0.32, // cws2
        301, // doy
        7.50872, // gmst
        23, // hour
        2.45367e+06, // julian_date
        0, // kGrain
        1e-05, // kLeaf
        0.15, // kRhizome
        0.15, // kRoot
        0.7, // kStem
        0.00636863, // lai
        5, // leafdeathrate
        171.066, // lha
        1.64205, // lmst
        0, // net_assimilation_rate_grain
        0, // net_assimilation_rate_leaf
        0, // net_assimilation_rate_rhizome
        0, // net_assimilation_rate_root
        0, // net_assimilation_rate_stem
        0, // precip
        0.75365, // rh
        369, // rhizome_senescence_index
        0, // root_senescence_index
        7.46476, // soil_evaporation_rate
        0.290028, // soil_water_content
        0.092, // solar
        217.63, // solar_L
        341.014, // solar_azimuth_angle
        -13.4789, // solar_dec
        215.874, // solar_ell
        23.4381, // solar_ep
        294.598, // solar_g
        -146.435, // solar_ra
        152.342, // solar_zenith_angle
        0, // stem_senescence_index
        5.175, // temp
        301.958, // time
        -6, // time_zone_offset
        100, // vmax
        2.66, // windspeed
        2005 // year
    };

    compare_last_row_with_expected(command_line, expected_value);
}


TEST(RegressionTests, TestWillow2002) {
    vector<string> command_line {"", "-d", "../app/sample_input/2002_weather.xml",
                                 "../app/sample_input/biocro-system.willow.xml"};

    // The EXPECT tests called in the compare_last_row_with_expected
    // function called below make use of the fact that
    // getElementsByTagName returns matching elements in document
    // order, which happens to be alphabetical order.
    vector<double> expected_value {
        0, // Grain
        5.26291e-07, // GrossAssim
        0.00615591, // Leaf
        0.427956, // LeafLitter
        0.821547, // LeafWS
        1.46428, // Rhizome
        0, // RhizomeLitter
        1.60745, // Root
        0, // RootLitter
        0, // Shell
        1.1, // Sp
        5.3346, // Stem
        0, // StemLitter
        0.750379, // StomataWS
        3867.71, // TTc
        0, // alpha
        -1.10745e-06, // canopy_assimilation_rate
        0.000408679, // canopy_transpiration_rate
        -0.84183, // cosine_zenith_angle
        0.200182, // cws1
        0.32, // cws2
        287, // doy
        6.57081, // gmst
        23, // hour
        2.45256e+06, // julian_date
        0, // kGrain
        1e-05, // kLeaf
        0.15, // kRhizome
        0.15, // kRoot
        0.7, // kStem
        0.0067715, // lai
        5, // leafdeathrate
        170.531, // lha
        0.704144, // lmst
        0, // net_assimilation_rate_grain
        0, // net_assimilation_rate_leaf
        0, // net_assimilation_rate_rhizome
        0, // net_assimilation_rate_root
        0, // net_assimilation_rate_stem
        0, // precip
        0.62275, // rh
        420, // rhizome_senescence_index
        0, // root_senescence_index
        7.52482, // soil_evaporation_rate
        0.290046, // soil_water_content
        0, // solar
        203.562, // solar_L
        342.452, // solar_azimuth_angle
        -8.44683, // solar_dec
        201.672, // solar_ell
        23.4386, // solar_ep
        280.582, // solar_g
        -159.969, // solar_ra
        147.334, // solar_zenith_angle
        0, // stem_senescence_index
        5.635, // temp
        287.958, // time
        -6, // time_zone_offset
        100, // vmax
        2.575, // windspeed
        2002 // year
    };

    compare_last_row_with_expected(command_line, expected_value);
}

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

    DOMNodeList* row_list = result.get_elements_by_tag_name("row");
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

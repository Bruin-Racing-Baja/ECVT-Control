// FUCK THESE MIGHT NOT BE THE RIGHT SIZE

#include <iostream>
using namespace std;

// data storage
#define DATA_LENGTH 10000
unsigned long millis[DATA_LENGTH];
unsigned long eg_ticks[DATA_LENGTH];
unsigned long gb_ticks[DATA_LENGTH];
unsigned int eg_rpm[DATA_LENGTH];
unsigned in gb_rpm[DATA_LENGTH];

// timing
const byte control_period = 20; // [ms]

// scaling constants
#define RPM_PREDIV 10
#define FILT_GAIN 1e2

// filters
const byte eg_filt_const_1 = 88;
const byte eg_filt_const_2 = -77;
const unsigned int eg_count2rpm = 1e3*60/control_period/RPM_PREDIV;
const byte gb_filt_const_1 = 56;
const byte gb_filt_const_2 = -11;
const unsigned int gb_count2rpm = 1e3*60*49/control_period/4/18/RPM_PREDIV;

void read_data_files();

int main() {

    // read encoder data


    // process data
    unsigned int eg_rpm_raw(0); // [rpm/10]
    unsigned int gb_rpm_raw(0); // [rpm/10]
    for (int i = 1; i < DATA_LENGTH; i++) {
        unsigned int eg_rpm_raw_prev = eg_rpm_raw;
        eg_rpm_raw = (eg_ticks[i] - eg_ticks[i-1]) * eg_count2rpm;
        eg_rpm[i] = eg_filt_const_1*(eg_rpm_raw + eg_rpm_raw_prev) + eg_filt_const_2*eg_rpm[i-1]/FILT_GAIN;

        unsigned int gb_rpm_raw_prev = gb_rpm_raw;
        gb_rpm_raw = (gb_ticks[i] - gb_ticks[i-1]) * gb_count2rpm;
        gb_rpm[i] = gb_filt_const_1*(gb_rpm_raw + gb_rpm_raw_prev) + gb_filt_const_2*eg_rpm[i-1]/FILT_GAIN;
    }

    // write speed data


    return(0);
}

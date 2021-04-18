#define NORTHBOUND_NUM 10 // Plus 1
#define SOUTHBOUND_NUM 10 // Plus 1
#define TOTALSTATIONS 21 // Plus 1

typedef struct
{
    String name;
    int passengers;
    double lat;
    double lng;
} route_type;

route_type route[TOTALSTATIONS] = {
    {"EDSA LRT", 0, 14.5366, 121.0004},
    {"Libertad", 0, 14.5420, 120.9925},
    {"Buendia", 0, 14.5525, 121.0338},
    {"Vito Cruz", 0, 14.5669, 121.0017},
    {"Quirino", 0, 14.5688, 120.9883},
    {"Pedro Gil", 0, 14.5726, 120.9861},
    {"PGH", 0, 14.5733, 120.9848},
    {"UN", 0, 14.5762, 120.9841},
    {"Manila City Hall", 0, 14.5898, 120.9816},
    {"Lawton", 0, 14.592279, 120.980253},   // INDEX 9 - NB END 
    {"Lawton", 0, 14.592279, 120.980253},   // INDEX 10 - SB - START
    {"Manila City Hall", 0, 14.5898, 120.9816},
    {"UN", 0, 14.5762, 120.9841},
    {"Pedro Gil", 0, 14.5726, 120.9861},
    {"Quirino", 0, 14.5688, 120.9883},
    {"Vito Cruz", 0, 14.5669, 121.0017},
    {"PICC", 0, 14.55473, 120.98306},
    {"Sofitel", 0, 14.55380, 120.98026},
    {"MOA", 0, 14.53536, 120.98265},
    {"EDSA LRT", 0, 14.5366, 121.0004},

};

// route_type northbound[NORTHBOUND_NUM] = {
//     {"EDSA LRT", 0, 14.5366, 121.0004},
//     {"Libertad", 0, 14.5420, 120.9925},
//     {"Buendia", 0, 14.5525, 121.0338},
//     {"Vito Cruz", 0, 14.5669, 121.0017},
//     {"Quirino", 0, 14.5688, 120.9883},
//     {"Pedro Gil", 0, 14.5726, 120.9861},
//     {"PGH", 0, 14.5733, 120.9848},
//     {"UN", 0, 14.5762, 120.9841},
//     {"Manila City Hall", 0, 14.5898, 120.9816},
//     {"Lawton", 0, 14.592279, 120.980253},
// };

// route_type southbound[SOUTHBOUND_NUM] = {
//     {"Lawton", 0, 14.592279, 120.980253},
//     {"Manila City Hall", 0, 14.5898, 120.9816},
//     {"UN", 0, 14.5762, 120.9841},
//     {"Pedro Gil", 0, 14.5726, 120.9861},
//     {"Quirino", 0, 14.5688, 120.9883},
//     {"Vito Cruz", 0, 14.5669, 121.0017},
//     {"PICC", 0, 14.55473, 120.98306},
//     {"Sofitel", 0, 14.55380, 120.98026},
//     {"MOA", 0, 14.53536, 120.98265},
//     {"EDSA LRT", 0, 14.5366, 121.0004},
// };
#pragma once

#include <cstdint>

namespace DistanceS {

    namespace FL {

        constexpr uint8_t address = 0x29;
        uint16_t getDist();
        void setHzReading();
    }

    namespace FC {

        constexpr uint8_t address = 0x30;
        uint16_t getDist();
        void setHzReading();
    }

    namespace FR {

        constexpr uint8_t address = 0x31;
        uint16_t getDist();
        void setHzReading();
    }

    namespace RL {

        constexpr uint8_t address = 0x32;
        uint16_t getDist();
        void setHzReading();
    }

    namespace RC {

        constexpr uint8_t address = 0x33;
        uint16_t getDist();
        void setHzReading();
    }

    namespace RR {

        constexpr uint8_t address = 0x34;
        uint16_t getDist();
        void setHzReading();
    }



    void initAll();

    struct Distances {

        uint16_t fl_dist;
        uint16_t fc_dist;
        uint16_t fr_dist;
        uint16_t rl_dist;
        uint16_t rc_dist;
        uint16_t rr_dist;

    }
    
    Distances getAllDistances();


};

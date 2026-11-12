#pragma once

#include <atomic>


/*
Para actualizar un dato se usa
dato.store(100);
Para leer un dato se usa
dato.load();
*/

namespace Global {

// RPM de los motores
namespace MTR {

    inline std::atomic<int> L_rpm{0};
    inline std::atomic<int> R_rpm{0};

    enum class IS {
        High,
        Medium,
        Low
    };
    inline std::atomic<IS> L_Ilevel{Low};
    inline std::atomic<IS> R_Ilevel{Low};
}

// Distancia de los VL53L0X
namespace DS {

    inline std::atomic<int> FL_dist{0};
    inline std::atomic<int> FC_dist{0};
    inline std::atomic<int> FR_dist{0};
    inline std::atomic<int> RL_dist{0};
    inline std::atomic<int> RC_dist{0};
    inline std::atomic<int> RR_dist{0};
}

// Estado de los QTR1A
namespace LS {

    inline std::atomic<bool> FL_state{false};
    inline std::atomic<bool> FR_state{false};
    inline std::atomic<bool> RL_state{false};
    inline std::atomic<bool> RR_state{false};
}


}

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <iomanip>
#include <map>

enum class MissionState {
    IDLE,
    SEARCHING,
    DETECTED,
    APPROACHING,
    MISSION_COMPLETE
};

std::string toString(MissionState state) {
    switch (state) {
        case MissionState::IDLE:             return "IDLE";
        case MissionState::SEARCHING:        return "SEARCHING";
        case MissionState::DETECTED:         return "DETECTED";
        case MissionState::APPROACHING:      return "APPROACHING";
        case MissionState::MISSION_COMPLETE: return "MISSION_COMPLETE";
    }
    return "UNKNOWN";
}


struct DetectionResult {
    std::string objectName;                 // kosong = tidak ada objek
    double confidence = 0.0;
    std::optional<double> distance;         // bisa tidak ada nilainya
};

class MissionManager {
public:
    MissionManager() : state_(MissionState::IDLE) {}

    // Dipanggil setiap kali sistem vision mengirim hasil deteksi baru
    MissionState updateState(const std::string& objectName,
                              double confidence,
                              std::optional<double> distance) {
        DetectionResult detection{objectName, confidence, distance};

        state_ = decideState(detection);
        history_.push_back({detection, state_});
        printReport(detection);

        return state_;
    }

    
    MissionState updateState() {
        return updateState("", 0.0, std::nullopt);
    }

    MissionState getState() const { return state_; }

    std::string getAction() const {
        static const std::map<MissionState, std::string> actions = {
            {MissionState::IDLE,             "STANDBY"},
            {MissionState::SEARCHING,        "SCAN AREA"},
            {MissionState::DETECTED,         "APPROACH TARGET"},
            {MissionState::APPROACHING,      "MOVE CLOSER"},
            {MissionState::MISSION_COMPLETE, "MISSION SUCCESS"}
        };
        return actions.at(state_);
    }

private:
    static constexpr double CONFIDENCE_THRESHOLD = 0.70;
    static constexpr double APPROACH_DISTANCE   = 2.0;
    static constexpr double COMPLETE_DISTANCE   = 0.5;

    MissionState state_;
    std::vector<std::pair<DetectionResult, MissionState>> history_;

    
    MissionState decideState(const DetectionResult& d) const {
        // Tidak ada objek sama sekali -> kembali mencari
        if (d.objectName.empty()) {
            return MissionState::SEARCHING;
        }

        
        if (d.confidence < CONFIDENCE_THRESHOLD) {
            return MissionState::SEARCHING;
        }

        
        MissionState result = MissionState::DETECTED;

        if (d.distance.has_value()) {
            double dist = d.distance.value();
            if (dist <= COMPLETE_DISTANCE) {
                result = MissionState::MISSION_COMPLETE;
            } else if (dist <= APPROACH_DISTANCE) {
                result = MissionState::APPROACHING;
            }
        }

        return result;
    }

    void printReport(const DetectionResult& d) const {
        std::cout << std::fixed << std::setprecision(2);

        if (state_ == MissionState::SEARCHING && d.objectName.empty()) {
            std::cout << "Object detected : None\n";
            std::cout << "Mission State   : " << toString(state_) << "\n";
            std::cout << "Action          : " << getAction() << "\n";
        } else {
            std::cout << "Object detected : " << d.objectName << "\n";
            std::cout << "Confidence      : " << d.confidence << "\n";
            std::cout << "Distance        : " << d.distance.value_or(0.0) << " m\n";
            std::cout << "Mission State   : " << toString(state_) << "\n";
            std::cout << "Action          : " << getAction() << "\n";
        }
        std::cout << std::string(42, '-') << "\n";
    }
};

int main() {
    MissionManager orca;

    // 1. Tidak ada objek terdeteksi -> SEARCHING
    orca.updateState();

    // 2. Objek terdeteksi, jarak masih jauh -> DETECTED
    orca.updateState("Gate", 0.85, 5.0);

    // 3. Jarak sudah <= 2.0 m -> APPROACHING
    orca.updateState("Gate", 0.90, 1.5);

    // 4. Jarak sudah <= 0.5 m -> MISSION_COMPLETE
    orca.updateState("Gate", 0.95, 0.4);

    // 5. Confidence terlalu rendah -> dianggap belum valid, SEARCHING
    orca.updateState("Flare", 0.40, 3.0);

    return 0;
}

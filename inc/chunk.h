
    std::string ToMapFileName(int mid) {
        std::stringstream ss;
        ss << "res/map/" << std::setfill('0') << std::setw(8)
            << std::hex << mid << ".map";
        return ss.str();
    }
    void LoadMap(int mid);
    void SaveMap(int mid);


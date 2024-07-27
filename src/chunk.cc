
void Game::LoadMap(int mid) {
    std::string mfn = ToMapFileName(mid);

    std::ifstream ifs(mfn, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Failed to open map." << std::endl;
        Quit();
    }

    ifs.seekg(0, std::ios::end);
    int file_size = ifs.tellg();
    int map_size = kMapHeight * kMapDepth * kMapWidth;
    if (file_size < map_size) {
        std::cerr << "Error: Failed to load map." << std::endl;
        Quit();
    }

    ifs.seekg(0);
    ifs.read(world_map_, map_size);
}

void Game::SaveMap(int mid) {
    std::string mfn = ToMapFileName(mid);

    std::ofstream ofs(mfn, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error: Failed to open map." << std::endl;
        Quit();
    }

    int map_size = kMapHeight * kMapDepth * kMapWidth;
    ofs.write(world_map_, map_size);
}


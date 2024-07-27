
const std::string Game::kTexDir = "bedrock-samples/resource_pack/textures/blocks/";
const std::array<std::string, Game::kNTexs> Game::kTexFiles = {
    "grass_carried.png",            // 0
    "grass_side_carried.png",       // 1
    "dirt.png",                     // 2
    "planks_big_oak.png",           // 3
    "quartz_block_chiseled_top.png",// 4
    "quartz_block_chiseled.png",    // 5
};
void Game::LoadTexs() {
    unsigned long tw, th, err = 0;
    for (int i = 0; kNTexs > i; i++) {
        texs_[i].resize(kTexWidth * kTexHeight);
        err |= QuickCG::loadImage(texs_[i], tw, th, kTexDir + kTexFiles[i]);
    }
   if (err) {
        std::cerr << "Error: Failed to load textures." << std::endl;
        Quit();
    }
}


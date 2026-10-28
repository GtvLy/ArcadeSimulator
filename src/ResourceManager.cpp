#include "ResourceManager.h"

sf::Texture& ResourceManager::loadTexture(const std::string& path) {
    // 如果已经加载过，直接返回
    if (textures.find(path) != textures.end()) {
        std::cout << "Texture already loaded: " << path << std::endl;
        return textures[path];
    }
    
    // 加载新纹理
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        std::cerr << "!!! CRITICAL ERROR: Failed to load texture: " << path << std::endl;
        std::cerr << "!!! Please check if the file exists!" << std::endl;
        
        // 创建一个10x10的红色纹理作为错误标记
        sf::Image errorImage;
        errorImage.create(10, 10, sf::Color::Red);
        texture.loadFromImage(errorImage);
        
        textures[path] = texture;
        return textures[path];
    }
    
    textures[path] = texture;
    std::cout << "Texture loaded successfully: " << path << std::endl;
    return textures[path];
}

sf::Texture& ResourceManager::getTexture(const std::string& path) {
    if (textures.find(path) == textures.end()) {
        std::cerr << "Texture not found, loading: " << path << std::endl;
        return loadTexture(path);
    }
    return textures[path];
}

sf::Font& ResourceManager::loadFont(const std::string& path) {
    if (fonts.find(path) != fonts.end()) {
        std::cout << "Font already loaded: " << path << std::endl;
        return fonts[path];
    }
    
    sf::Font font;
    if (!font.loadFromFile(path)) {
        std::cerr << "Failed to load font: " << path << std::endl;
        fonts[path] = sf::Font();
        return fonts[path];
    }
    
    fonts[path] = font;
    std::cout << "Font loaded successfully: " << path << std::endl;
    return fonts[path];
}

sf::Font& ResourceManager::getFont(const std::string& path) {
    if (fonts.find(path) == fonts.end()) {
        std::cerr << "Font not found, loading: " << path << std::endl;
        return loadFont(path);
    }
    return fonts[path];
}

sf::SoundBuffer& ResourceManager::loadSoundBuffer(const std::string& path) {
    if (soundBuffers.find(path) != soundBuffers.end()) {
        std::cout << "Sound buffer already loaded: " << path << std::endl;
        return soundBuffers[path];
    }
    
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path)) {
        std::cerr << "Failed to load sound buffer: " << path << std::endl;
        soundBuffers[path] = sf::SoundBuffer();
        return soundBuffers[path];
    }
    
    soundBuffers[path] = buffer;
    std::cout << "Sound buffer loaded successfully: " << path << std::endl;
    return soundBuffers[path];
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(const std::string& path) {
    if (soundBuffers.find(path) == soundBuffers.end()) {
        std::cerr << "Sound buffer not found, loading: " << path << std::endl;
        return loadSoundBuffer(path);
    }
    return soundBuffers[path];
}

sf::Music& ResourceManager::loadMusic(const std::string& path) {
    if (music.find(path) != music.end()) {
        std::cout << "Music already loaded: " << path << std::endl;
        return *music[path];
    }
    
    auto newMusic = std::make_unique<sf::Music>();
    if (!newMusic->openFromFile(path)) {
        std::cerr << "Failed to load music: " << path << std::endl;
        music[path] = std::make_unique<sf::Music>();
        return *music[path];
    }
    
    music[path] = std::move(newMusic);
    std::cout << "Music loaded successfully: " << path << std::endl;
    return *music[path];
}

sf::Music& ResourceManager::getMusic(const std::string& path) {
    if (music.find(path) == music.end()) {
        std::cerr << "Music not found, loading: " << path << std::endl;
        return loadMusic(path);
    }
    return *music[path];
}

void ResourceManager::clear() {
    textures.clear();
    fonts.clear();
    soundBuffers.clear();
    music.clear();
    std::cout << "All resources cleared!" << std::endl;
}

bool ResourceManager::hasTexture(const std::string& path) const {
    return textures.find(path) != textures.end();
}

bool ResourceManager::hasFont(const std::string& path) const {
    return fonts.find(path) != fonts.end();
}

bool ResourceManager::hasSoundBuffer(const std::string& path) const {
    return soundBuffers.find(path) != soundBuffers.end();
}

bool ResourceManager::hasMusic(const std::string& path) const {
    return music.find(path) != music.end();
}
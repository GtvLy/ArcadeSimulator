#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>
#include <iostream>

class ResourceManager {
private:
    // 使用map存储资源，键为资源路径
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Font> fonts;
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, std::unique_ptr<sf::Music>> music;  // Music需要用指针
    
    // 单例模式
    ResourceManager() {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

public:
    // 获取单例实例
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    // 加载纹理
    sf::Texture& loadTexture(const std::string& path);
    
    // 获取纹理（不加载）
    sf::Texture& getTexture(const std::string& path);
    
    // 加载字体
    sf::Font& loadFont(const std::string& path);
    
    // 获取字体
    sf::Font& getFont(const std::string& path);
    
    // 加载音效
    sf::SoundBuffer& loadSoundBuffer(const std::string& path);
    
    // 获取音效
    sf::SoundBuffer& getSoundBuffer(const std::string& path);
    
    // 加载音乐（背景音乐）
    sf::Music& loadMusic(const std::string& path);
    
    // 获取音乐
    sf::Music& getMusic(const std::string& path);
    
    // 清空所有资源
    void clear();
    
    // 检查资源是否已加载
    bool hasTexture(const std::string& path) const;
    bool hasFont(const std::string& path) const;
    bool hasSoundBuffer(const std::string& path) const;
    bool hasMusic(const std::string& path) const;
};

#endif
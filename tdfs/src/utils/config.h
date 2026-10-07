#ifndef CONFIG_H
#define CONFIG_H

namespace tdfs {
  class Config {
    public:
      Config(const Config&) = delete;
      Config& operator=(const Config&) = delete;
      Config(Config&&) = delete;
      Config& operator=(Config&&) = delete;

      static Config& getInstance();
      
  private:
      Config() = default;
  };
}

#endif

--TEST--
Asymmetric visibility with property hooks - public private(namespace)(set)
--FILE--
<?php

namespace App\Config {
    class Settings {
        public private(namespace)(set) int $value {
            get => $this->value * 2;
            set => $value + 10;
        }
        
        public function __construct() {
            $this->value = 5;
        }
    }
    
    class ConfigManager {
        public function update(Settings $settings): void {
            // Same namespace - can set
            $settings->value = 20;
            var_dump($settings->value);
        }
    }
}

namespace App\Controllers {
    $settings = new \App\Config\Settings();
    // Different namespace - can read (public)
    var_dump($settings->value);
    
    $manager = new \App\Config\ConfigManager();
    $manager->update($settings);
    
    // Different namespace - cannot write (private(namespace)(set))
    $settings->value = 100;
}

?>
--EXPECTF--
int(30)
int(60)

Fatal error: Uncaught Error: Cannot modify private(namespace)(set) property App\Config\Settings::$value from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d

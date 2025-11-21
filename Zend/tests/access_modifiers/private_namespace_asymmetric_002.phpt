--TEST--
Asymmetric visibility: public private(namespace)(set) property - write from same namespace
--FILE--
<?php

namespace App {
    class Config {
        public private(namespace)(set) int $value = 100;
    }

    class ConfigManager {
        public function setValue(): void {
            $config = new Config();
            $config->value = 200;
            echo $config->value . "\n";
        }
    }

    $manager = new ConfigManager();
    $manager->setValue();
}

?>
--EXPECT--
200

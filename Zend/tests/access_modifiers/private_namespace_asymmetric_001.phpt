--TEST--
Asymmetric visibility: public private(namespace)(set) property - read from same namespace
--FILE--
<?php

namespace App {
    class Config {
        public private(namespace)(set) int $value = 100;
    }

    class ConfigManager {
        public function getValue(): int {
            $config = new Config();
            return $config->value;
        }
    }

    $manager = new ConfigManager();
    echo $manager->getValue() . "\n";
}

?>
--EXPECT--
100

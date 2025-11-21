--TEST--
Asymmetric visibility: public private(namespace)(set) property - read from different namespace succeeds
--FILE--
<?php

namespace App {
    class Config {
        public private(namespace)(set) int $value = 100;
    }
}

namespace Other {
    class Consumer {
        public function readValue(): int {
            $config = new \App\Config();
            return $config->value;
        }
    }

    $consumer = new Consumer();
    echo $consumer->readValue() . "\n";
}

?>
--EXPECT--
100

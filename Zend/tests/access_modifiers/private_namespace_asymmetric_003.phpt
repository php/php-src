--TEST--
Asymmetric visibility: public private(namespace)(set) property - write from different namespace fails
--FILE--
<?php

namespace App {
    class Config {
        public private(namespace)(set) int $value = 100;
    }
}

namespace Other {
    class Consumer {
        public function tryWrite(): void {
            $config = new \App\Config();
            $config->value = 200;
        }
    }

    $consumer = new Consumer();
    $consumer->tryWrite();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify private(namespace)(set) property App\Config::$value from scope Other\Consumer in %s:%d
Stack trace:
#0 %s(%d): Other\Consumer->tryWrite()
#1 {main}
  thrown in %s on line %d

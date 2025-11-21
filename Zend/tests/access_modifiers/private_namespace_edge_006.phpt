--TEST--
private(namespace) method can be used as callable from same namespace
--FILE--
<?php

namespace App {
    class Service {
        private(namespace) function process(int $x): int {
            return $x * 2;
        }
    }

    class Consumer {
        public function testCallable(): void {
            $service = new Service();
            $callable = [$service, 'process'];
            echo call_user_func($callable, 21) . "\n";
        }
    }

    $consumer = new Consumer();
    $consumer->testCallable();
}

?>
--EXPECT--
42

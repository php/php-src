--TEST--
Traits: private(namespace) method from trait can't be accessed from different namespace
--FILE--
<?php

namespace App {
    trait ServiceTrait {
        private(namespace) function helper(): string {
            return "helped";
        }
    }

    class Service {
        use ServiceTrait;
    }
}

namespace Other {
    class Consumer {
        public function test(): void {
            $service = new \App\Service();
            $service->helper();
        }
    }

    $consumer = new Consumer();
    $consumer->test();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private(namespace) method App\Service::helper() from scope Other\Consumer in %s:%d
Stack trace:
#0 %s(%d): Other\Consumer->test()
#1 {main}
  thrown in %s on line %d

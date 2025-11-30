--TEST--
private(namespace) access from sub-namespace fails (exact namespace match required)
--FILE--
<?php

namespace App {
    class Service {
        private(namespace) function test(): string {
            return "success";
        }
    }
}

namespace App\Sub {
    class Consumer {
        public function callTest(): void {
            $service = new \App\Service();
            $service->test();
        }
    }

    $consumer = new Consumer();
    $consumer->callTest();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private(namespace) method App\Service::test() from scope App\Sub\Consumer in %s:%d
Stack trace:
#0 %s(%d): App\Sub\Consumer->callTest()
#1 {main}
  thrown in %s on line %d

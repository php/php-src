--TEST--
private(namespace) in global namespace blocks access from namespaced code
--FILE--
<?php

namespace {
    class GlobalService {
        private(namespace) function test(): string {
            return "global";
        }
    }
}

namespace App {
    class Consumer {
        public function callTest(): void {
            $service = new \GlobalService();
            $service->test();
        }
    }

    $consumer = new Consumer();
    $consumer->callTest();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private(namespace) method GlobalService::test() from scope App\Consumer in %s:%d
Stack trace:
#0 %s(%d): App\Consumer->callTest()
#1 {main}
  thrown in %s on line %d

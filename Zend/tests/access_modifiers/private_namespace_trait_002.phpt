--TEST--
Traits: private(namespace) method uses receiver class namespace (different from trait namespace)
--FILE--
<?php

namespace Traits {
    trait LoggerTrait {
        private(namespace) function log(string $msg): void {
            echo "Logged: $msg\n";
        }
    }
}

namespace App {
    use Traits\LoggerTrait;

    class Service {
        use LoggerTrait;

        public function doWork(): void {
            // This should work because the method belongs to App namespace (receiver class)
            $this->log("working");
        }
    }

    $service = new Service();
    $service->doWork();
}

?>
--EXPECT--
Logged: working

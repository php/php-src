--TEST--
Traits: private(namespace) method uses receiver class namespace
--FILE--
<?php

namespace App\Utils {
    trait LoggerTrait {
        private(namespace) function log(string $msg): void {
            echo "Logged: $msg\n";
        }
    }
}

namespace App\Utils {
    class Service {
        use LoggerTrait;

        public function doWork(): void {
            $this->log("working");
        }
    }

    $service = new Service();
    $service->doWork();
}

?>
--EXPECT--
Logged: working

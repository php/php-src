--TEST--
private(namespace) method access from same namespace (same class)
--FILE--
<?php

namespace App\Services {
    class UserService {
        private(namespace) function validate(): string {
            return "validated";
        }

        public function process(): string {
            return $this->validate();
        }
    }

    $service = new UserService();
    echo $service->process() . "\n";
}

?>
--EXPECT--
validated

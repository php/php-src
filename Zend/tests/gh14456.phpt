--TEST--
GH-14456: Attempting to initialize class with private constructor calls destructor
--FILE--
<?php

class PrivateUser {
    private function __construct() {
        return new stdClass();
    }
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

try {
    new PrivateUser();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Destructor for PrivateUser
Error: Call to private PrivateUser::__construct() from global scope

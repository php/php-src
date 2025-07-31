--TEST--
GH-14456: Attempting to initialize class with private constructor calls destructor
--FILE--
<?php

class PrivateUser {
    private function __construct() {}
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
Error: Call to private PrivateUser::__construct() from global scope

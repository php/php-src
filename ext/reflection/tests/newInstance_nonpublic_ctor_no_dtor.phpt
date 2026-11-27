--TEST--
ReflectionClass::newInstance(Args)() must not call the destructor when the constructor is not public
--FILE--
<?php

class A {
    private function __construct() {}

    public function __destruct() {
        echo "dtor\n";
    }
}

$rc = new ReflectionClass(A::class);
try {
    $rc->newInstance();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
try {
    $rc->newInstanceArgs();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
ReflectionException: Access to non-public constructor of class A
ReflectionException: Access to non-public constructor of class A

--TEST--
Bug #38217 (ReflectionClass::newInstanceArgs() tries to allocate too much memory)
--FILE--
<?php

class ObjectOne {
    public function __construct() {
    }
}

$class= new ReflectionClass('ObjectOne');
var_dump($class->newInstanceArgs());

class ObjectTwo {
    public function __construct($var) {
        var_dump($var);
    }
}

$class= new ReflectionClass('ObjectTwo');
try {
    var_dump($class->newInstanceArgs());
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
var_dump($class->newInstanceArgs(array('test')));


echo "Done\n";
?>
--EXPECTF--
object(ObjectOne)#%d (0) {
}
Exception: Too few arguments to function ObjectTwo::__construct(), 0 passed and exactly 1 expected
string(4) "test"
object(ObjectTwo)#%d (0) {
}
Done

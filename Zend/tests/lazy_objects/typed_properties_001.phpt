--TEST--
Typed property assignment by ref with variable name on proxy
--FILE--
<?php

class Test {
    public int $prop;
}

$name = new class {
    public function __toString() {
        return 'prop';
    }
};

$reflector = new ReflectionClass(Test::class);
$test = $reflector->newLazyProxy(function () {
    return new Test();
});
$ref = "foobar";
try {
    $test->$name =& $ref;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test);

?>
--EXPECTF--
Cannot assign string to property Test::$prop of type int
lazy proxy object(Test)#%d (1) {
  ["instance"]=>
  object(Test)#%d (0) {
    ["prop"]=>
    uninitialized(int)
  }
}

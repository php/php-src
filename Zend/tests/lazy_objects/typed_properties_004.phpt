--TEST--
Lazy Objects: Foreach by ref over typed properties
--FILE--
<?php

class C {
    public int $a = 1;
    private int $_b = 1;
    public int $b {
        &get { $value = &$this->_b; return $value; }
    }
}

$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyProxy(function () {
    return new C();
});

foreach ($obj as $key => &$value) {
    var_dump($key);
    try {
        $value = 'string';
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
    $value = 2;
}

var_dump($obj);

?>
--EXPECTF--
string(1) "a"
TypeError: Cannot assign string to reference held by property C::$a of type int
string(1) "b"
TypeError: Cannot assign string to reference held by property C::$_b of type int
lazy proxy object(C)#%d (1) {
  ["instance"]=>
  object(C)#%d (2) {
    ["a"]=>
    int(2)
    ["_b":"C":private]=>
    &int(2)
  }
}

--TEST--
Cannot append to ArrayObject if backing value is an object
--EXTENSIONS--
spl
--FILE--
<?php

class MyClass {
    private $foo1;
    protected $foo2;

    public function read() {
        var_dump($this->foo1, $this->foo2);
    }
}
$c = new MyClass();
$ao = new ArrayObject($c);

try {
    $ao['foo1'] = 'bar1';
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $ao['foo2'] = 'bar2';
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

var_dump($c);
$c->read();
var_dump($ao['foo1']);
var_dump($ao['foo2']);
try {
    var_dump($c->foo1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($c->foo2);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
Error: Cannot access private property MyClass::$foo1
Error: Cannot access protected property MyClass::$foo2
object(MyClass)#1 (2) {
  ["foo1":"MyClass":private]=>
  NULL
  ["foo2":protected]=>
  NULL
}
NULL
NULL

Warning: Undefined array key "foo1" in %s on line %d
NULL

Warning: Undefined array key "foo2" in %s on line %d
NULL
Error: Cannot access private property MyClass::$foo1
Error: Cannot access protected property MyClass::$foo2

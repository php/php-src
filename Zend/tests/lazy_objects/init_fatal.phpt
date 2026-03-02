--TEST--
Lazy objects: fatal error during initialization of ghost object
--FILE--
<?php

class C {
    public $a;
    public $b;
    public $c;
    public function __construct() {
        eval('{');
    }
}

$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

$reflector->getProperty('b')->setRawValueWithoutLazyInitialization($obj, new stdClass);

var_dump($obj);
var_dump($obj->c);
var_dump($obj);

--EXPECTF--
lazy ghost object(C)#%d (1) {
  ["b"]=>
  object(stdClass)#%d (0) {
  }
}
string(11) "initializer"

Parse error: Unclosed '{' in %s on line %d

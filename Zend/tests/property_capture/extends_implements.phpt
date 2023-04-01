--TEST--
Property capture with parent class and interface
--FILE--
<?php

class Base {
    public $baseProperty = 1;
    public function baseMethod() {
        return 2;
    }
}

interface Face {
    public function requiredMethod();
}

$captured = 3;
$anon = new class use($captured) extends Base implements Face {
    public function requiredMethod() {
        return 4;
    }
};

var_dump($anon->baseProperty);
var_dump($anon->baseMethod());
var_dump($anon->captured);
var_dump($anon->requiredMethod());

var_dump($anon instanceof Base);
var_dump($anon instanceof Face);


?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
bool(true)
bool(true)

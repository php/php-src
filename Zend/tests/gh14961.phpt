--TEST--
GH-14961: Comments after -> operator
--FILE--
<?php

class C {
    public $prop;
}

$c = new C();
$c->/* comment */prop = 42;
var_dump($c->/** doc comment */prop);
var_dump($c->
    // line comment
    prop);
var_dump($c->
    # hash comment
    prop);
var_dump($c?->/* comment */prop);

?>
--EXPECT--
int(42)
int(42)
int(42)
int(42)

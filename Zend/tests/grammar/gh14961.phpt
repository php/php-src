--TEST--
GH-14961: Comment between -> and keyword
--FILE--
<?php

class C {
    public $class = C::class;
}

$c = new C();
$c->/* comment */class = 42;
var_dump($c->/** doc comment */class);
var_dump($c->
    // line comment
    class);
var_dump($c->
    # hash comment
    class);
var_dump($c?->/* comment */class);

?>
--EXPECT--
int(42)
int(42)
int(42)
int(42)

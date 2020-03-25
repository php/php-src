--TEST--
Operators on overlaoded property reference
--FILE--
<?php
class C {
    function __construct() { $this->bar = str_repeat("1", 2); }
    function &__get($x) { return $this->bar; }
    function __set($x, $v) { $this->bar = $v; }
}
$x = new C;
var_dump(++$x->foo);
$x = new C;
var_dump($x->foo++);
$x = new C;
var_dump($x->foo += 2);
?>
--EXPECT--
int(12)
string(2) "11"
int(13)

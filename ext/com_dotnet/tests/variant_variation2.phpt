--TEST--
Testing reading and writing of properties
--EXTENSIONS--
com_dotnet
--FILE--
<?php
class MyClass {
    public $foo = "foo";
    public string $bar = "bar";
}

$o = new MyClass();
$v = new variant($o);
var_dump($v->foo);
var_dump($v->bar);
$v->foo = "new foo";
var_dump($v->foo instanceof variant);
var_dump((string) $v->foo);
var_dump($o->foo instanceof variant);
var_dump((string) $o->foo);
$v->bar = "new bar";
var_dump($v->bar);
var_dump($o->bar);
?>
--EXPECT--
string(3) "foo"
string(3) "bar"
bool(true)
string(7) "new foo"
bool(true)
string(7) "new foo"
string(7) "new bar"
string(7) "new bar"

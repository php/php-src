--TEST--
Testing reading properties and calling functions
--EXTENSIONS--
com_dotnet
--FILE--
<?php
class MyClass {
    public $foo = "property";
    public $bar = "bar";
    public function foo() {
        return "method";
    }
    public function stdClass() {
        return new stdclass();
    }
}

$o = new MyClass();
$v = new variant($o);
var_dump($v->foo);
var_dump($v->foo());
var_dump($v->bar);
var_dump($v->bar());
var_dump($v->stdclass);
var_dump($v->stdclass());
try {
    var_dump($v->qux);
} catch (com_exception $ex) {
    echo $ex->getMessage(), "\n";
}
?>
--EXPECTF--
string(6) "method"
string(6) "method"
string(3) "bar"
string(3) "bar"
object(variant)#%d (0) {
}
object(variant)#%d (0) {
}
Unable to lookup `qux': %s

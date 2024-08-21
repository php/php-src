--TEST--
Destructor call of autoloader when object freed
--FILE--
<?php
class A {
    public $var = 1;
    public function autoload() {
        echo "var:".$this->var."\n";
    }
    public function __destruct() {
        echo "__destruct__\n";
    }
}

$a = new A;
$a->var = 2;

autoload_register_function(array($a, 'autoload'));
unset($a);

var_dump(function_exists("C", true));
?>
===DONE===
--EXPECT--
var:2
bool(false)
===DONE===
__destruct__

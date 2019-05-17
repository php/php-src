--TEST--
SPL: spl_autoload() and object freed
--INI--
include_path=.
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

spl_autoload_register(array($a, 'autoload'));
unset($a);

var_dump(class_exists("C", true));
?>
===DONE===
<?php exit(0); ?>
--EXPECT--
var:2
bool(false)
===DONE===
__destruct__

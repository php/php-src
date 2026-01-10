--TEST--
OSS Fuzz #61865: Undef variable in ++/-- for declared property that is unset in error handler
--FILE--
<?php
class C {
    public $a;
    function errorHandler($errno, $errstr) {
        unset($this->a);
    }
}
$c = new C;
set_error_handler([$c,'errorHandler']);
unset($c->a);
$c->a += 5;
var_dump($c->a);
?>
--EXPECT--
int(5)

--TEST--
OSS Fuzz #61469: Undef variable in ++/-- for dynamic property that is unset in error handler
--FILE--
<?php
class C {
    function errorHandle() {
        unset($this->a);
    }
}
$c = new C;
set_error_handler([$c,'errorHandle']);
$c->a += 5;
var_dump($c->a);
?>
--EXPECT--
int(5)

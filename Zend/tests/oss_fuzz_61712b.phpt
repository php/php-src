--TEST--
OSS-Fuzz #61712 (assertion failure with error handler during binary op)
--FILE--
<?php
#[AllowDynamicProperties]
class C {
    function error($_, $msg) {
        echo $msg, "\n";
        $this->a = 12345;
    }
}

$c = new C;
set_error_handler([$c, 'error']);
$c->a %= 10;
var_dump($c->a);
?>
--EXPECT--
Undefined property: C::$a
int(5)

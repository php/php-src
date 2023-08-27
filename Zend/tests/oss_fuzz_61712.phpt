--TEST--
OSS-Fuzz #61712
--FILE--
<?php
#[AllowDynamicProperties]
class C {
    function error($_, $msg) {
        echo $msg, "\n";
        unset($this->a);
    }
}

$c = new C;
set_error_handler([$c, 'error']);
$c->a %= 10;
var_dump($c->a);
?>
--EXPECT--
Undefined property: C::$a
int(0)

--TEST--
Static type is not allowed in parameters
--FILE--
<?php

// TODO: We could prohibit this case in the compiler instead.

class Test {
    public function test(static $param) {
    }
}

?>
--EXPECTF--
Parse error: syntax error, unexpected 'static' (T_STATIC), expecting variable (T_VARIABLE) in %s on line %d

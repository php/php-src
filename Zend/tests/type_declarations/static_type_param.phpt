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
Parse error: syntax error, unexpected token "static", expecting variable in %s on line %d

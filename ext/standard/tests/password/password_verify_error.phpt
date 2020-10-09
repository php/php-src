--TEST--
Test error operation of password_verify()
--FILE--
<?php
//-=-=-=-

try {
    var_dump(password_verify("foo"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
password_verify() expects exactly 2 arguments, 1 given

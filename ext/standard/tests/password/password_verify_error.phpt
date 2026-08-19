--TEST--
Test error operation of password_verify()
--FILE--
<?php
//-=-=-=-

try {
    var_dump(password_verify("foo"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ArgumentCountError: password_verify() expects exactly 2 arguments, 1 given

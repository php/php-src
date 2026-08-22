--TEST--
Test password_verify() - argument errors
--FILE--
<?php

try {
    password_verify('password');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ArgumentCountError: password_verify() expects exactly 2 arguments, 1 given

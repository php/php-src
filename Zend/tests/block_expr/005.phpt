--TEST--
Test exceptions in block expression
--SKIPIF--
<?php
die("skip this test because it leaks memory");
?>
--FILE--
<?php

try {
    $ary = [];
    ($ary + [1]) + { throw new Exception('a'); null };
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

try {
    throw new Exception({ throw new Exception('b'); null });
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(1) "a"
string(1) "b"

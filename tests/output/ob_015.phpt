--TEST--
output buffering - failure
--FILE--
<?php
ob_start("str_rot13", 1);
try {
    echo "foo\n";
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
ob_end_flush();
?>
--EXPECT--
foo
ArgumentCountError: str_rot13() expects exactly 1 argument, 2 given

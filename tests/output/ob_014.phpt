--TEST--
output buffering - failure
--FILE--
<?php
ob_start("str_rot13");
echo "foo\n";
try {
    ob_end_flush();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
foo
ArgumentCountError: str_rot13() expects exactly 1 argument, 2 given

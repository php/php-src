--TEST--
output buffering - failure
--FILE--
<?php
ob_start("str_rot13", 1);
try {
    echo "foo\n";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
ob_end_flush();
?>
--EXPECT--
foo
str_rot13(): Exactly 1 argument is expected, 2 given

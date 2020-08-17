--TEST--
output buffering - failure
--FILE--
<?php
ob_start("str_rot13");
echo "foo\n";
try {
    ob_end_flush();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
foo
str_rot13(): Exactly 1 argument is expected, 2 given

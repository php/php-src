--TEST--
output buffering - failure
--FILE--
<?php
ob_start("str_rot13");
echo "foo\n";
// str_rot13 expects 1 param and returns NULL when passed 2 params.
// It is invoked with 2 params when used as an OB callback.
// Therefore, there will be no data in the buffer. This is expected: see bug 46900.
ob_end_flush();

// Show the error.
print_r(error_get_last());
?>
--EXPECTF--
Array
(
    [type] => 2
    [message] => str_rot13() expects exactly 1 parameter, 2 given
    [file] => %s
    [line] => 7
)

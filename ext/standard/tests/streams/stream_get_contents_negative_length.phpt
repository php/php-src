--TEST--
stream_get_contents() with negative max length
--FILE--
<?php

$tmp = tmpfile();
fwrite($tmp, "abcd");
var_dump(stream_get_contents($tmp, 2, 1));

try {
    stream_get_contents($tmp, -2);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
string(2) "bc"
stream_get_contents(): Argument #2 ($length) must be greater than or equal to -1

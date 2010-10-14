--TEST--
stream_get_contents() - Testing offset out of range
--FILE--
<?php

$tmp = tmpfile();

fwrite($tmp, b"12345");

echo stream_get_contents($tmp, 2, 5), "--\n";
echo stream_get_contents($tmp, 2), "--\n";
fseek($tmp, 0);
echo stream_get_contents($tmp, 2, 3), "--\n";
echo stream_get_contents($tmp, 2, -1), "--\n";

@unlink($tmp);

?>
--EXPECTF--
--
--
45--

Warning: stream_get_contents(): Number of bytes to seek must be non-negative, given -1 in %s on line %d
--

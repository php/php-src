--TEST--
stream_get_contents() - Testing offset out of range
--FILE--
<?php

$tmp = tmpfile();

fwrite($tmp, b"12345");

echo stream_get_contents($tmp, 2, 5), "--\n";
echo stream_get_contents($tmp, 2), "--\n";
echo stream_get_contents($tmp, 2, 3), "--\n";
echo stream_get_contents($tmp, 2, -1), "--\n";

@unlink($tmp);

?>
--EXPECT--
--
--
45--
--

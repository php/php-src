--TEST--
Bug #46426 (3rd parameter offset of stream_get_contents not works for "0")
--FILE--
<?php

$tmp = tmpfile();

fwrite($tmp, "12345");

echo stream_get_contents($tmp, -1, 0);
echo "\n";
echo stream_get_contents($tmp, -1, 1);
echo "\n";
echo stream_get_contents($tmp, -1, 2);

@unlink($tmp);

?>
--EXPECT--
12345
2345
345

--TEST--
Bug #46426 (3rd parameter offset of stream_get_contents not works for "0")
--FILE--
<?php

$tmp = tmpfile();

fwrite($tmp, "12345");

echo stream_get_contents($tmp, 2, 1);
echo "\n";
echo stream_get_contents($tmp, -1);
echo "\n";
echo stream_get_contents($tmp, -1, 0);
echo "\n";
echo stream_get_contents($tmp, -1, 2);
echo "\n";
echo stream_get_contents($tmp, 0, 0);
echo "\n";
echo stream_get_contents($tmp, 1, 0);
echo "\n";
echo stream_get_contents($tmp, -1);

@unlink($tmp);

?>
--EXPECT--
23
45
12345
345

1
2345

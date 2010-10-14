--TEST--
Bug #46426 (3rd parameter offset of stream_get_contents not works for "0")
--FILE--
<?php

$tmp = tmpfile();

fwrite($tmp, b"12345");

fseek($tmp, 0);
echo stream_get_contents($tmp, 2, 1); //23
echo "\n";
echo stream_get_contents($tmp, -1); //45
echo "\n";
fseek($tmp, -1, SEEK_CUR);
echo stream_get_contents($tmp, -1, 0); //5
echo "\n";
fseek($tmp, 0);
echo stream_get_contents($tmp, -1, 2); //345
echo "\n";
fseek($tmp, 0);
echo stream_get_contents($tmp, 0, 0); //""
echo "\n";
echo stream_get_contents($tmp, 1, 0); //1
echo "\n";
echo stream_get_contents($tmp, -1); //2345

@unlink($tmp);

?>
--EXPECT--
23
45
5
345

1
2345

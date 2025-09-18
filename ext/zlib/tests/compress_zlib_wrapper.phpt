--TEST--
compress.zlib:// wrapper
--EXTENSIONS--
zlib
--FILE--
<?php
chdir(__DIR__. "/data");

// Relative path
$fp = fopen("compress.zlib://test.txt.gz", "rb");
fclose($fp);

// Absolute path
$fp = fopen("compress.zlib://". __DIR__. "/data/test.txt.gz", "rb");
fclose($fp);

echo "ok\n";
?>
--EXPECT--
ok

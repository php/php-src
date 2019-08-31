--TEST--
zlib.inflate of gzip-encoded stream
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php

$a = gzopen(__DIR__ . '/test.txt.gz', 'w');
fwrite($a, "This is quite the thing ain't it\n");
fclose($a);

$fp = fopen(__DIR__ . '/test.txt.gz', 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);
echo fread($fp, 2000);
fclose($fp);
echo "1\n";
$fp = fopen(__DIR__ . '/test.txt.gz', 'r');
// zlib format
$fp = fopen(__DIR__ . '/test.txt.gz', 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ, array('window' => 15+16));
echo "2\n";
echo fread($fp, 2000);
fclose($fp);
// auto-detect
$fp = fopen(__DIR__ . '/test.txt.gz', 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ, array('window' => 15+32));
echo "3\n";
echo fread($fp, 2000);
fclose($fp);

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/test.txt.gz');
?>
--EXPECT--
1
2
This is quite the thing ain't it
3
This is quite the thing ain't it

--TEST--
gzgetss — Get line from gz-file pointer and strip HTML tags - function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php
if(!extension_loaded("zlib")){die("skip - ZLIB extension not loaded");}
?>
--FILE--
<?php
$handle = gzopen('gzgetss.gz', 'r');
while (!gzeof($handle)) {
   $buffer = gzgetss($handle, 4096);
   echo $buffer;
}
gzclose($handle);
?>
--EXPECT--
string gzgetss(resource $zp, int $length [, string $allowable_tags ]);

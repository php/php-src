--TEST--
gzgetss — Get line from gz-file pointer and strip HTML tags - function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php

<?php
if(!extension_loaded("zlib")){die("skip - ZLIB extension not loaded");}
?>
--FILE--
<?php
$handle = gzopen(__DIR__ . '/gzgetss.gz', 'r');

while (!gzeof($handle)){
   $buffer = gzgetss($handle, 4096, "<code>");
   print($buffer);
}
gzclose($handle);
?>
--EXPECT--
<code>stringgzgetss(resource $zp, int $length [, string $allowable_tags ]);<code/>
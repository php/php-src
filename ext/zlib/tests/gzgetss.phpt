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
while (!gzeof($handle)) {
   $buffer = gzgetss($handle, 100, "<h1>");
   echo $buffer;
}
gzclose($handle);
?>
--EXPECT--
<h1 class="title">Description</h1>
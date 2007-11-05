--TEST--
Crash inside stream_get_line(), when length=0
--FILE--
<?php
$path = dirname(__FILE__) . '/test.html';

file_put_contents($path, b"foo<br>bar<br>foo");
$fp = fopen($path, "r");
while ($fp && !feof($fp)) {
	echo stream_get_line($fp, 0, "<br>")."\n";
}
fclose($fp);
@unlink($path);
?>
--EXPECT--
foo
bar
foo

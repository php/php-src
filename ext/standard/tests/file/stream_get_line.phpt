--TEST--
Crash inside stream_get_line(), when length=0
--FILE--
<?php
$path = __DIR__ . '/test.html';

file_put_contents($path, "foo<br>bar<br>foo");
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

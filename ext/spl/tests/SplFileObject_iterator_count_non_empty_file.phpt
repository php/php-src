--TEST--
GH-19942 (iterator_count() on a non-empty SplFileObject should return correct count)
--FILE--
<?php
$tmpfile = tempnam(sys_get_temp_dir(), 'spl_non_empty_test');
file_put_contents($tmpfile, "line1\nline2\nline3");

$fileObject = new SplFileObject($tmpfile, 'r');
$count = iterator_count($fileObject);

var_dump($count);

unlink($tmpfile);
?>
--EXPECT--
int(3)

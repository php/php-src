--TEST--
GH-19942 (iterator_count() on an empty SplFileObject should return 0)
--FILE--
<?php
$tmpfile = tempnam(sys_get_temp_dir(), 'spl_empty_test');
touch($tmpfile);

$fileObject = new SplFileObject($tmpfile, 'r');
$count = iterator_count($fileObject);

var_dump($count);

unlink($tmpfile);
?>
--EXPECT--
int(1)

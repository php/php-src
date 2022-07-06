--TEST--
Bug #72447: Type Confusion in php_bz2_filter_create()
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php
$input = "AAAAAAAA";
$param = array('blocks' => $input);

$fp = fopen('testfile', 'w');
stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE, $param);
fclose($fp);
?>
--CLEAN--
<?php
unlink('testfile');
?>
--EXPECTF--
Warning: stream_filter_append(): Invalid parameter given for number of blocks to allocate. (0) in %s%ebug72447.php on line %d

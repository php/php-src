--TEST--
Bug #72447: Type Confusion in php_bz2_filter_create()
--EXTENSIONS--
bz2
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
Warning: stream_filter_append(): Number of blocks parameter must be of type int, string given in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "bzip2.compress" in %s on line %d

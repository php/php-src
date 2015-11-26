--TEST--
Bug #52732 (Docs say preg_match() returns FALSE on error, but it returns int(0))
--INI--
pcre.backtrack_limit=1
--FILE--
<?php
$ret = preg_match('/(?:\D+|<\d+>)*[!?]/', 'foobar foobar foobar');

var_dump($ret);

?>
--EXPECT--
bool(false)
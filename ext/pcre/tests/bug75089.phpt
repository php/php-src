--TEST--
Bug #75089 (preg_grep() is not reporting PREG_BAD_UTF8_ERROR after first input string)
--FILE--
<?php
preg_grep('#\d#u', ['a', "1\xFF"/*, 'c'*/]);
var_dump(preg_last_error());
?>
--EXPECT--
int(4)

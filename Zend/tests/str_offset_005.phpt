--TEST--
string offset 005 indirect string modification by error handler
--FILE--
<?php
set_error_handler(function(){$GLOBALS['a']=8;});
$a='a';
var_dump($a[$b]);
var_dump($a);
?>
--EXPECT--
string(1) "a"
int(8)

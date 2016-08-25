--TEST--
Bug #72538 (readline_redisplay crashes php)
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip"); 
if (READLINE_LIB != "libedit") die("skip libedit only");
?>
--FILE--
<?php 

readline_redisplay();

?>
okey
--EXPECT--
okey

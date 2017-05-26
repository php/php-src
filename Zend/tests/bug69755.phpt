--TEST--
Bug #69755: segfault in ZEND_CONCAT_SPEC_TMPVAR_CONST_HANDLER
--FILE--
<?php
c . 10;
?>
--EXPECTF--
Warning: Use of undefined constant c - assumed 'c' (this will throw an Error in a future version of PHP) in %sbug69755.php on line 2

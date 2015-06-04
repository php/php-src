--TEST--
Bug #69755: segfault in ZEND_CONCAT_SPEC_TMPVAR_CONST_HANDLER
--FILE--
<?php
c . 10;
?>
--EXPECTF--
Notice: Use of undefined constant c - assumed 'c' in %sbug69755.php on line 2

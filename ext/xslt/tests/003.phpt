--TEST--
Pass object for xslt_error_handler, bug #17931
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include('xslt_set_error_handler.php');
?>
--EXPECT--
OK

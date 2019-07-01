--TEST--
Bug #73654: Segmentation fault in zend_call_function
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo xyz();

function x () : string {
    return 'x';
}

function xyz() : string {
    return x().'yz';
}

?>
--EXPECT--
xyz

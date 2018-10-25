--TEST--
Bug #76752 (Crash in ZEND_COALESCE_SPEC_TMP_HANDLER - assertion in _get_zval_ptr_tmp failed)
--FILE--
<?php
$obj = new stdClass;
$val = 'foo';
$obj->prop = &$val;
var_dump($obj->prop ?? []);
?>
--EXPECT--
string(3) "foo"

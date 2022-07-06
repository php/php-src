--TEST--
Bug #78644: SEGFAULT in ZEND_UNSET_OBJ_SPEC_VAR_CONST_HANDLER
--FILE--
<?php

$a = new stdClass;
unset($a->b->c->d);
unset($a->b->c['d']);
var_dump($a);

?>
--EXPECT--
object(stdClass)#1 (1) {
  ["b"]=>
  NULL
}

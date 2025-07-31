--TEST--
Bug #70262 (Accessing array crashes)
--FILE--
<?php

$array = array();
$array[] = 1; // make this not immutable

$extra = $array; // make the refcount == 2

class A {
    public function getObj($array) {
        $obj = new Stdclass;
        $obj->arr = $array; // make the refcount == 3;
        return $obj;
    }
}

$a = new A;
$a->getObj($array) //use function call to get a refcount == 1 IS_VAR object
    ->arr // FETCH_OBJ_W will EXTRACT_ZVAL_PTR because getObj() result a refcount == 1 object (READY_TO_DESTROY)
        [0] = "test"; //We will get a refcount == 3 array (not a IS_INDIRCT) in ZEND_ASSIGN_DIM_SPEC_VAR_CONST_HANDLER

var_dump($array);
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}

--TEST--
Validation of attribute repetition (is allowed; userland attribute)
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL|Attribute::IS_REPEATABLE)]
class MyAttribute {}

#[ClassAlias('Other', [new MyAttribute(), new MyAttribute()])]
class Demo {}

$attributes = new ReflectionClassAlias('Other')->getAttributes();
var_dump($attributes);
$attributes[0]->newInstance();

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(10) "ClassAlias"
  }
}
php: /usr/src/php/Zend/zend_variables.c:143: zval_copy_ctor_func: Assertion `!(zval_gc_flags(((*(zvalue)).value.str)->gc.u.type_info) & (1<<6))' failed.
Aborted (core dumped)

Termsig=6

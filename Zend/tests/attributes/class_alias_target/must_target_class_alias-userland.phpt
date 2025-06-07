--TEST--
Error when attribute does not target class alias (useland attribute)
--FILE--
<?php

#[Attribute(Attribute::TARGET_FUNCTION)]
class MyFunctionAttribute {}

#[ClassAlias('Other', [new MyFunctionAttribute()])]
class Demo {}

$ref = new ReflectionClassAlias('Other');
$attribs = $ref->getAttributes();
var_dump($attribs);
$attribs[0]->newInstance();

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(19) "MyFunctionAttribute"
  }
}

Fatal error: Uncaught Error: Attribute "MyFunctionAttribute" cannot target class alias (allowed targets: function) in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %s on line %d

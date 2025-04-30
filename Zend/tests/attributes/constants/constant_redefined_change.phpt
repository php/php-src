--TEST--
If a constant is redefined, attributes remain unchanged (different attributes)
--FILE--
<?php

#[\MyAttribute]
const MY_CONST = "Has attributes (1)";

#[\MyOtherAttribute]
const MY_CONST = "Has attributes (2)";

echo MY_CONST . "\n";

$reflection = new ReflectionConstant('MY_CONST');
var_dump($reflection->getAttributes())

?>
--EXPECTF--
Warning: Constant MY_CONST already defined in %s on line %d
Has attributes (1)
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(11) "MyAttribute"
  }
}

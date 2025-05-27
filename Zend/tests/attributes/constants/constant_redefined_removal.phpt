--TEST--
If a constant is redefined, attributes remain unchanged (had attributes)
--FILE--
<?php

#[\MyAttribute]
const MY_CONST = "Has attributes";

const MY_CONST = "No attributes";

echo MY_CONST . "\n";

$reflection = new ReflectionConstant('MY_CONST');
var_dump($reflection->getAttributes())

?>
--EXPECTF--
Warning: Constant MY_CONST already defined in %s on line %d
Has attributes
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(11) "MyAttribute"
  }
}

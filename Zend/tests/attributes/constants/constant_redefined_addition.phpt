--TEST--
If a constant is redefined, attributes remain unchanged (no attributes)
--FILE--
<?php

const MY_CONST = "No attributes";

#[\MyAttribute]
const MY_CONST = "Has attributes";

echo MY_CONST . "\n";

$reflection = new ReflectionConstant('MY_CONST');
var_dump($reflection->getAttributes())

?>
--EXPECTF--
Warning: Constant MY_CONST already defined in %s on line %d
No attributes
array(0) {
}

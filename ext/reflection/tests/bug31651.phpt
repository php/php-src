--TEST--
Bug #31651 (ReflectionClass::getDefaultProperties segfaults with arrays.)
--FILE--
<?php

class Test
{
	var $a = array('a' => 1);
}

//$obj = new Test;
//$ref = new ReflectionClass($obj);
$ref = new ReflectionClass('Test');

print_r($ref->getdefaultProperties());

?>
--EXPECT--
Array
(
    [a] => Array
    (
        [a] => 1
    )
)

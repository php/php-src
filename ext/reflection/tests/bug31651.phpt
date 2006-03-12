--TEST--
Reflection Bug #31651 (ReflectionClass::getDefaultProperties segfaults with arrays.)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class Test
{
	public $a = array('a' => 1);
}

$ref = new ReflectionClass('Test');

print_r($ref->getDefaultProperties());

?>
--EXPECT--
Array
(
    [a] => Array
        (
            [a] => 1
        )

)

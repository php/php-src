--TEST--
Bug #52861 (unset failes with ArrayObject and deep arrays)
--FILE--
<?php
$arrayObject = new ArrayObject(array('foo' => array('bar' => array('baz' => 'boo'))));

unset($arrayObject['foo']['bar']['baz']);
print_r($arrayObject->getArrayCopy());
?>
--EXPECT--
Array
(
    [foo] => Array
        (
            [bar] => Array
                (
                )

        )

)

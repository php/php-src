--TEST--
SPL: ArrayObject serialize with an object as storage
--FILE--
<?php
$obj1 = new ArrayObject(new ArrayObject(array(1,2)));
$s = serialize($obj1);
$obj2 = unserialize($s);

print_r($obj1);
echo "$s\n";
print_r($obj2);
?>
--EXPECTF--
Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d

Deprecated: ArrayObject::__unserialize(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
ArrayObject Object
(
    [storage:ArrayObject:private] => ArrayObject Object
        (
            [storage:ArrayObject:private] => Array
                (
                    [0] => 1
                    [1] => 2
                )

        )

)
O:11:"ArrayObject":4:{i:0;i:0;i:1;O:11:"ArrayObject":4:{i:0;i:0;i:1;a:2:{i:0;i:1;i:1;i:2;}i:2;a:0:{}i:3;N;}i:2;a:0:{}i:3;N;}
ArrayObject Object
(
    [storage:ArrayObject:private] => ArrayObject Object
        (
            [storage:ArrayObject:private] => Array
                (
                    [0] => 1
                    [1] => 2
                )

        )

)

--TEST--
Bug #69955 (Segfault when trying to combine [] and assign-op on ArrayAccess object).
--FILE--
<?php
class C10 implements ArrayAccess
{
        function offsetExists($offset): bool
        {
                echo "\nInside " . __METHOD__ . "\n"; var_dump($offset);
        }
        function offsetGet($offset): mixed
        {
                echo "\nInside " . __METHOD__ . "\n"; var_dump($offset); return 100;
        }
        function offsetSet($offset, $value): void
        {
                echo "\nInside " . __METHOD__ . "\n"; var_dump($offset); var_dump($value);
        }
        function offsetUnset($offset): void
        {
                echo "\nInside " . __METHOD__ . "\n"; var_dump($offset);
        }
}

$c10 = new C10;

var_dump($c10[] += 5);
?>
--EXPECT--
Inside C10::offsetGet
NULL

Inside C10::offsetSet
NULL
int(105)
int(105)

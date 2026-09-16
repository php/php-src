--TEST--
Errors from __unserialize() with invalid data
--FILE--
<?php

echo "ArrayObject:\n";

try {
    // empty array
    unserialize('O:11:"ArrayObject":0:{}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:11:"ArrayObject":3:{i:0;b:1;i:1;a:0:{}i:2;a:0:{}}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:11:"ArrayObject":3:{i:0;i:0;i:1;a:0:{}i:2;i:0;}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:11:"ArrayObject":3:{i:0;i:0;i:1;i:0;i:2;a:0:{}}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    // iterator class name is not a string
    unserialize('O:11:"ArrayObject":4:{i:0;i:0;i:1;i:0;i:2;a:0:{}i:3;i:0;}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:11:"ArrayObject":4:{i:0;i:0;i:1;a:2:{i:0;i:1;i:1;i:2;}i:2;a:0:{}i:3;s:11:"NonExistent";}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

class Existent {}

try {
    unserialize('O:11:"ArrayObject":4:{i:0;i:0;i:1;a:2:{i:0;i:1;i:1;i:2;}i:2;a:0:{}i:3;s:8:"Existent";}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "ArrayIterator:\n";

try {
    unserialize('O:13:"ArrayIterator":0:{}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:13:"ArrayIterator":3:{i:0;b:1;i:1;a:0:{}i:2;a:0:{}}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:13:"ArrayIterator":3:{i:0;i:0;i:1;a:0:{}i:2;i:0;}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:13:"ArrayIterator":3:{i:0;i:0;i:1;i:0;i:2;a:0:{}}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "SplDoublyLinkedList:\n";

try {
    unserialize('O:19:"SplDoublyLinkedList":0:{}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:19:"SplDoublyLinkedList":3:{i:0;b:1;i:1;a:0:{}i:2;a:0:{}}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:19:"SplDoublyLinkedList":3:{i:0;i:0;i:1;a:0:{}i:2;i:0;}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:19:"SplDoublyLinkedList":3:{i:0;i:0;i:1;i:0;i:2;a:0:{}}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "SplObjectStorage:\n";

try {
    unserialize('O:16:"SplObjectStorage":0:{}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:16:"SplObjectStorage":2:{i:0;i:0;i:1;a:0:{}}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:16:"SplObjectStorage":2:{i:0;a:0:{}i:1;i:1;}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:16:"SplObjectStorage":2:{i:0;a:1:{i:0;i:0;}i:1;a:0:{}}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unserialize('O:16:"SplObjectStorage":2:{i:0;a:2:{i:0;i:0;i:1;i:0;}i:1;a:0:{}}');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ArrayObject:
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Incomplete or ill-typed serialization data
InvalidArgumentException: Passed variable is not an array or object
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Cannot deserialize ArrayObject with iterator class 'NonExistent'; no such class exists
UnexpectedValueException: Cannot deserialize ArrayObject with iterator class 'Existent'; this class is not derived from ArrayIterator
ArrayIterator:
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Incomplete or ill-typed serialization data
InvalidArgumentException: Passed variable is not an array or object
SplDoublyLinkedList:
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Incomplete or ill-typed serialization data
SplObjectStorage:
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Incomplete or ill-typed serialization data
UnexpectedValueException: Odd number of elements
UnexpectedValueException: Non-object key

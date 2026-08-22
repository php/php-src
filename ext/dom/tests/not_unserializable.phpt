--TEST--
DOM classes are not unserializable
--EXTENSIONS--
dom
--FILE--
<?php

$classes = [
    "DOMXPath",
    "DOMDocument",
    "DOMNode",
    "DOMNameSpaceNode",
];

foreach ($classes as $class)
{
    try {
        unserialize('O:' . strlen($class) . ':"' . $class . '":0:{}');
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Exception: Unserialization of 'DOMXPath' is not allowed
Exception: Unserialization of 'DOMDocument' is not allowed, unless unserialization methods are implemented in a subclass
Exception: Unserialization of 'DOMNode' is not allowed, unless unserialization methods are implemented in a subclass
Exception: Unserialization of 'DOMNameSpaceNode' is not allowed, unless unserialization methods are implemented in a subclass

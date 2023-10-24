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
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Unserialization of 'DOMXPath' is not allowed
Unserialization of 'DOMDocument' is not allowed, unless unserialization methods are implemented in a subclass
Unserialization of 'DOMNode' is not allowed, unless unserialization methods are implemented in a subclass
Unserialization of 'DOMNameSpaceNode' is not allowed, unless unserialization methods are implemented in a subclass

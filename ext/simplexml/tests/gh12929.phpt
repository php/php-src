--TEST--
GH-12929 (SimpleXMLElement with stream_wrapper_register can segfault)
--EXTENSIONS--
simplexml
--FILE--
<?php
$scheme = "foo1";
stream_wrapper_register($scheme, "SimpleXMLIterator");
try {
    file_get_contents($scheme . "://x");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
    echo $e->getPrevious()->getMessage(), "\n";
}

$scheme = "foo2";
stream_wrapper_register($scheme, "SimpleXMLElement");
try {
    file_get_contents($scheme . "://x");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
    echo $e->getPrevious()->getMessage(), "\n";
}
?>
--EXPECT--
It's not possible to assign a complex type to properties, resource given
SimpleXMLElement is not properly initialized
It's not possible to assign a complex type to properties, resource given
SimpleXMLElement is not properly initialized

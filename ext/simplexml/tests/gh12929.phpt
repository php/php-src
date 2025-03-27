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
    var_dump($e->getPrevious());
}

$scheme = "foo2";
stream_wrapper_register($scheme, "SimpleXMLElement");
try {
    file_get_contents($scheme . "://x");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
    var_dump($e->getPrevious());
}
?>
--EXPECT--
SimpleXMLElement is not properly initialized
NULL
SimpleXMLElement is not properly initialized
NULL

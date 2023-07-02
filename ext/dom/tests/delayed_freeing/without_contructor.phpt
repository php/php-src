--TEST--
Tests without running the constructor
--EXTENSIONS--
dom
--FILE--
<?php
$rc = new ReflectionClass('DOMNode');
$node = $rc->newInstanceWithoutConstructor();

// Property read test
try {
    var_dump($node);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

// Call test
try {
    $node->removeChild($node);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

// Import test
$doc = new DOMDocument;
try {
    $doc->appendChild($doc->importNode($node));
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(DOMNode)#2 (0) {
}
Invalid State Error
Couldn't fetch DOMNode
Couldn't fetch DOMNode

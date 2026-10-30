--TEST--
GH-16190 (Using reflection to call Dom\Node::__construct causes assertion failure)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = Dom\XMLDocument::createEmpty();
$rm = new ReflectionMethod($doc, '__construct');
try {
    $rm->invoke($doc);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot directly construct Dom\XMLDocument, use document methods instead

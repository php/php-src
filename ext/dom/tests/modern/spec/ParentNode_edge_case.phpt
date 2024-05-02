--TEST--
HierarchyRequestError on appending document to element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$container = $dom->createElement('container');
try {
    $container->append($dom);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Hierarchy Request Error

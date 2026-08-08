--TEST--
HierarchyRequestError on appending document to element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$container = $dom->createElement('container');
try {
    $container->append($dom);
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Hierarchy Request Error

--TEST--
Test property write errors
--EXTENSIONS--
dom
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument();

try {
    $dom->nodeValue = [];
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $dom->nodeType += 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $dom->xmlEncoding = null;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$entity = new DOMEntity();

try {
    $entity->actualEncoding = null;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $entity->encoding = null;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $entity->version = null;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot assign array to property DOMNode::$nodeValue of type ?string
Cannot modify readonly property DOMDocument::$nodeType
Cannot modify readonly property DOMDocument::$xmlEncoding
Cannot modify readonly property DOMEntity::$actualEncoding
Cannot modify readonly property DOMEntity::$encoding
Cannot modify readonly property DOMEntity::$version

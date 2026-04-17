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
Cannot modify private(set) property DOMNode::$nodeType from global scope
Cannot modify private(set) property DOMDocument::$xmlEncoding from global scope
Cannot modify private(set) property DOMEntity::$actualEncoding from global scope
Cannot modify private(set) property DOMEntity::$encoding from global scope
Cannot modify private(set) property DOMEntity::$version from global scope

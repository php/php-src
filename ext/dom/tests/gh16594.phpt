--TEST--
GH-16594 (Assertion failure in DOM -> before)
--EXTENSIONS--
dom
--FILE--
<?php

$v1 = new DOMText("wr");
$v2 = new DOMDocument();
$v6 = new DOMComment("aw");
$v7 = new DOMAttr("r", "iL");

$v9 = $v2->createElement("test");
$v9->setAttributeNodeNS($v7);
$v7->appendChild($v1);

try {
    $v1->before($v6);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Hierarchy Request Error

--TEST--
Not returning a node set for C14N
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument;
$dom->loadXML('<root>test</root>');
try {
    $dom->documentElement->C14N(true, false, ['query' => 'string(./root/text())']);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
XPath query did not return a nodeset

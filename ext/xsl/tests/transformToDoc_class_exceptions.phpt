--TEST--
XSLTProcessor::transformToDoc class exceptions
--EXTENSIONS--
xsl
simplexml
dom
--FILE--
<?php

$sxe = simplexml_load_file(__DIR__ . '/53965/collection.xml');

$processor = new XSLTProcessor;
$dom = new DOMDocument;
$dom->load(__DIR__ . '/53965/collection.xsl');
$processor->importStylesheet($dom);

try {
    $processor->transformToDoc($sxe, NonExistent::class);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $processor->transformToDoc($sxe, DOMDocument::class);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
XSLTProcessor::transformToDoc(): Argument #2 ($returnClass) must be a valid class name or null, NonExistent given
XSLTProcessor::transformToDoc(): Argument #2 ($returnClass) must be a class name compatible with SimpleXMLElement, DOMDocument given

--TEST--
XSLTProcessor::transformToDoc() with a return class that cannot be instantiated
--EXTENSIONS--
xsl
dom
simplexml
--FILE--
<?php

abstract class AbstractDocument extends DOMDocument {}
abstract class AbstractElement extends SimpleXMLElement {}
class UndefinedConstantDocument extends DOMDocument {
    public $value = UNDEFINED_CONSTANT;
}

$stylesheet = new DOMDocument;
$stylesheet->loadXML('<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:template match="/"><out/></xsl:template></xsl:stylesheet>');
$processor = new XSLTProcessor;
$processor->importStylesheet($stylesheet);

$dom = new DOMDocument;
$dom->loadXML('<root/>');
$sxe = simplexml_load_string('<root/>');
$modern = Dom\XMLDocument::createFromString('<root/>');

foreach ([[$dom, AbstractDocument::class], [$sxe, AbstractElement::class], [$dom, UndefinedConstantDocument::class], [$modern, Dom\Document::class]] as [$input, $class]) {
    try {
        $processor->transformToDoc($input, $class);
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}

define('UNDEFINED_CONSTANT', 'defined');
$result = $processor->transformToDoc($dom, UndefinedConstantDocument::class);
echo $result::class, ": ", $result->value, PHP_EOL;
echo $result->saveXML();

?>
--EXPECT--
Error: Cannot instantiate abstract class AbstractDocument
Error: Cannot instantiate abstract class AbstractElement
Error: Undefined constant "UNDEFINED_CONSTANT"
Error: Cannot instantiate abstract class Dom\Document
UndefinedConstantDocument: defined
<?xml version="1.0"?>
<out/>

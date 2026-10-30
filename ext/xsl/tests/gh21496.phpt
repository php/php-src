--TEST--
GH-21496 (UAF in dom_objects_free_storage when importing non-document node as stylesheet)
--EXTENSIONS--
dom
xsl
--CREDITS--
YuanchengJiang
--FILE--
<?php
$comment = new DOMComment("my value");
$doc = new DOMDocument();
$doc->loadXML(<<<XML
<container/>
XML);
$doc->documentElement->appendChild($comment);
unset($doc);
$proc = new XSLTProcessor();
var_dump($proc->importStylesheet($comment));
$sxe = simplexml_load_string('<container/>');
$proc = new XSLTProcessor();
$proc->importStylesheet($sxe);
?>
--EXPECTF--
Warning: XSLTProcessor::importStylesheet(): compilation error: file %s line 1 element container in %s on line %d

Warning: XSLTProcessor::importStylesheet(): xsltParseStylesheetProcess : document is not a stylesheet in %s on line %d
bool(false)

Warning: XSLTProcessor::importStylesheet(): compilation error: element container in %s on line %d

Warning: XSLTProcessor::importStylesheet(): xsltParseStylesheetProcess : document is not a stylesheet in %s on line %d


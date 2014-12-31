--TEST--
bug #49634 (Segfault throwing an exception in a XSL registered function)
--SKIPIF--
<?php
extension_loaded("xsl") or die("skip need ext/xsl");
?>
--FILE--
<?php

$sXml = <<<XML
<?xml version="1.0" encoding="UTF-8" ?>
<root>
	test
</root>
XML;

$cDIR = __DIR__;
$sXsl = <<<XSL
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:ext="http://php.net/xsl"
                xsl:extension-element-prefixes="ext"
                exclude-result-prefixes="ext">
	<xsl:output encoding="UTF-8" indent="yes" method="xml" />
	<xsl:template match="/">
		<xsl:value-of select="ext:function('testFunction', document('$cDIR/bug49634.xml')/root)"/>
	</xsl:template>
</xsl:stylesheet>
XSL;

function testFunction($a)
{
		throw new Exception('Test exception.');
}

$domXml = new DOMDocument;
$domXml->loadXML($sXml);
$domXsl = new DOMDocument;
$domXsl->loadXML($sXsl);

for ($i = 0; $i < 10; $i++)
{
	$xsltProcessor = new XSLTProcessor();
	$xsltProcessor->registerPHPFunctions(array('testFunction'));
	$xsltProcessor->importStyleSheet($domXsl);
	try {
		@$xsltProcessor->transformToDoc($domXml);
	} catch (Exception $e) {
		echo $e,"\n";
	}
}
?>
===DONE===
--EXPECTF--
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
exception 'Exception' with message 'Test exception.' in %s:%d
Stack trace:
#0 [internal function]: testFunction(Array)
#1 %s(%d): XSLTProcessor->transformToDoc(Object(DOMDocument))
#2 {main}
===DONE===

--TEST--
GH-22621 (UAF via XSLTProcessor::registerPHPFunctions() retaining namespace nodes from an external document)
--EXTENSIONS--
dom
xsl
--CREDITS--
ExPatch-LLC
--FILE--
<?php
$cDIR = __DIR__;
file_put_contents($cDIR . '/gh22621_external.xml', '<root xmlns:custom="urn:custom"><data>secret</data></root>');
$uri = 'file:///' . ltrim(str_replace('\\', '/', $cDIR), '/') . '/gh22621_external.xml';

$stored_ns = null;

function capture_ns($nodes) {
    global $stored_ns;
    foreach ($nodes as $node) {
        if ($node instanceof DOMNameSpaceNode) {
            $stored_ns = $node;
        }
    }
    return "captured";
}

$xsl = new DOMDocument();
$xsl->loadXML(<<<XSL
<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:php="http://php.net/xsl">
  <xsl:template match="/">
    <result>
      <xsl:value-of select="php:function('capture_ns', document('$uri')/*/namespace::*)"/>
    </result>
  </xsl:template>
</xsl:stylesheet>
XSL);

$doc = new DOMDocument();
$doc->loadXML('<input/>');

$proc = new XSLTProcessor();
$proc->registerPHPFunctions();
$proc->importStylesheet($xsl);
echo $proc->transformToXml($doc);

var_dump($stored_ns->localName);
var_dump($stored_ns->namespaceURI);
var_dump($stored_ns->parentNode->localName);
var_dump($stored_ns->ownerDocument instanceof DOMDocument);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh22621_external.xml');
?>
--EXPECTF--
<?xml version="1.0"?>
<result xmlns:php="http://php.net/xsl">captured</result>
string(6) "custom"
string(10) "urn:custom"
string(4) "root"
bool(true)

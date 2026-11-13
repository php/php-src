--TEST--
XSLTProcessor: the source becomes writable again after a fatal error in user code during a transformation
--EXTENSIONS--
xsl
zend_test
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('skip LeakSanitizer reports the libxslt state that the bailout abandons');
?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root><a/></root>');

register_shutdown_function(function () use ($doc) {
    $doc->documentElement->firstChild->remove();
    echo $doc->saveXML($doc->documentElement), "\n";
});

set_error_handler(function () use ($doc) {
    try {
        $doc->documentElement->firstChild->remove();
    } catch (DOMException $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    zend_trigger_bailout();
});

$xsl = new DOMDocument();
$xsl->loadXML('<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:output method="text"/><xsl:template match="/"><xsl:message>m</xsl:message></xsl:template></xsl:stylesheet>');
$proc = new XSLTProcessor();
$proc->importStylesheet($xsl);
$proc->transformToXml($doc);
?>
--EXPECTF--
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress

Fatal error: Bailout in %s on line %d
<root/>

--TEST--
XSLTProcessor: documents become writable again after a fatal error in a php:function() callback
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
$doc->loadXML('<root><a>1</a></root>');
$other = new DOMDocument();
$other->loadXML('<other><x/></other>');

function other_root() {
    return $GLOBALS['other']->documentElement;
}

function bail() {
    try {
        $GLOBALS['other']->documentElement->firstChild->remove();
    } catch (DOMException $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    zend_trigger_bailout();
}

function stylesheet(string $select): DOMDocument {
    $xsl = new DOMDocument();
    $xsl->loadXML('<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl"><xsl:output method="text"/><xsl:template match="/"><xsl:value-of select="' . $select . '"/></xsl:template></xsl:stylesheet>');
    return $xsl;
}

$proc = new XSLTProcessor();
$proc->registerPHPFunctions();
$proc->importStylesheet(stylesheet("concat(count(php:function('other_root')), php:function('bail'))"));

register_shutdown_function(function () use ($proc, $doc, $other) {
    $doc->documentElement->firstChild->remove();
    $other->documentElement->firstChild->remove();
    echo $doc->saveXML($doc->documentElement), $other->saveXML($other->documentElement), "\n";
    $proc->importStylesheet(stylesheet("count(php:function('other_root'))"));
    echo $proc->transformToXml($doc), "\n";
});

$proc->transformToXml($doc);
?>
--EXPECTF--
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress

Fatal error: Bailout in %s on line %d
<root/><other/>
1

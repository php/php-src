--TEST--
XSLTProcessor: an entity loader cannot modify the document that is being transformed
--EXTENSIONS--
xsl
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root><a>1</a><a>2</a><a>3</a></root>');

libxml_set_external_entity_loader(function ($public, $system, $context) use ($doc) {
    try {
        $doc->documentElement->textContent = '';
    } catch (DOMException $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    $stream = fopen('php://memory', 'w+');
    fwrite($stream, '<x>loaded</x>');
    rewind($stream);
    return $stream;
});

$xsl = new DOMDocument();
$xsl->loadXML(<<<XSL
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text"/>
<xsl:template match="/">
<xsl:for-each select="//a">[<xsl:value-of select="document('urn:x')/x"/>|<xsl:value-of select="."/>]</xsl:for-each>
</xsl:template>
</xsl:stylesheet>
XSL);
$proc = new XSLTProcessor();
$proc->importStylesheet($xsl);
var_dump($proc->transformToXml($doc));
$doc->documentElement->textContent = '';
echo $doc->saveXML($doc->documentElement), "\n";
?>
--EXPECT--
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
string(30) "[loaded|1][loaded|2][loaded|3]"
<root></root>

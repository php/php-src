--TEST--
XSLTProcessor: cycle collection triggered while the php:function node list is torn down
--EXTENSIONS--
dom
xsl
--FILE--
<?php
class GcElement extends DOMElement
{
    private static int $destroyed = 0;

    public function __destruct()
    {
        /* Collect once the node list teardown has already freed some entries. */
        if (++self::$destroyed === 3) {
            gc_collect_cycles();
        }
    }
}

$xml = new DOMDocument();
$xml->loadXML('<root><a/><b/><c/><d/><e/><f/><g/><h/></root>');
$xml->registerNodeClass(DOMElement::class, GcElement::class);

$xsl = new DOMDocument();
$xsl->loadXML(<<<XSL
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl">
<xsl:template match="/"><out><xsl:for-each select="/root/*"><xsl:value-of select="php:function('cb', .)"/></xsl:for-each></out></xsl:template>
</xsl:stylesheet>
XSL);

function cb(array $nodes): string
{
    return $nodes[0]->nodeName;
}

$proc = new XSLTProcessor();
$proc->registerPHPFunctions();
$proc->importStylesheet($xsl);

$root_buffer = $proc;
unset($root_buffer);

echo $proc->transformToXml($xml);
echo 'done', PHP_EOL;
?>
--EXPECT--
<?xml version="1.0"?>
<out xmlns:php="http://php.net/xsl">abcdefgh</out>
done

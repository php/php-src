--TEST--
XSLTProcessor callback node list is reported to the cycle collector
--EXTENSIONS--
dom
xsl
--FILE--
<?php
$xml = new DOMDocument();
$xml->loadXML('<root><a/></root>');

$xsl = new DOMDocument();
$xsl->loadXML(<<<XSL
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl">
<xsl:template match="/"><out><xsl:value-of select="php:function('hold', /root/a)"/><xsl:value-of select="php:function('pause')"/></out></xsl:template>
</xsl:stylesheet>
XSL);

function hold(array $nodes): string {
    @$nodes[0]->fiber = $GLOBALS['fiber'];
    return 'h';
}

function pause(): string {
    Fiber::suspend();
    return 'p';
}

$proc = new XSLTProcessor();
$proc->registerPHPFunctions();
$proc->importStylesheet($xsl);
$wr = WeakReference::create($proc);

/* Suspending inside a callback leaves the transform without reaching the
   node list cleanup, so the list still holds the node that owns the fiber. */
$fiber = new Fiber(static function () use ($proc, $xml) {
    $proc->transformToXml($xml);
});
$GLOBALS['fiber'] = $fiber;
$fiber->start();

var_dump($fiber->isSuspended());

unset($proc, $fiber, $GLOBALS['fiber'], $xml, $xsl);
gc_collect_cycles();

var_dump($wr->get() === null);
?>
--EXPECT--
bool(true)
bool(true)

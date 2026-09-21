--TEST--
XSLTProcessor::importStylesheet() rejects a stylesheet whose __clone() retains a node of the cloned document
--EXTENSIONS--
dom
xsl
--FILE--
<?php
class RetainsElement extends DOMDocument {
    public function __clone(): void {
        $GLOBALS['stash'] = $this->documentElement;
    }
}

$doc = new RetainsElement;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/"><out/></xsl:template>
</xsl:stylesheet>
XML);

$proc = new XSLTProcessor();
try {
    var_dump($proc->importStylesheet($doc));
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
$kept = $GLOBALS['stash'];
unset($GLOBALS['stash'], $proc, $doc);
echo get_class($kept), " is still usable: ", $kept->nodeName, PHP_EOL;
?>
--EXPECT--
ValueError: XSLTProcessor::importStylesheet(): Argument #1 ($stylesheet) must not have its clone retained by __clone()
DOMElement is still usable: xsl:stylesheet

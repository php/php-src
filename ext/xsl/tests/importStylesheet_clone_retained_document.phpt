--TEST--
XSLTProcessor::importStylesheet() rejects a stylesheet whose __clone() retains the cloned document
--EXTENSIONS--
dom
xsl
--FILE--
<?php
const STYLESHEET = <<<XML
<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/"><out/></xsl:template>
</xsl:stylesheet>
XML;

class Harmless extends DOMDocument {
    public function __clone(): void {
    }
}

class RetainsDocument extends DOMDocument {
    public function __clone(): void {
        $GLOBALS['stash'] = $this;
    }
}

$doc = new Harmless;
$doc->loadXML(STYLESHEET);
$proc = new XSLTProcessor();
var_dump($proc->importStylesheet($doc));
unset($proc, $doc);

$doc = new RetainsDocument;
$doc->loadXML(STYLESHEET);
$proc = new XSLTProcessor();
try {
    var_dump($proc->importStylesheet($doc));
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
$kept = $GLOBALS['stash'];
unset($GLOBALS['stash'], $proc, $doc);
echo get_class($kept), " is still usable: ", $kept->documentElement->nodeName, PHP_EOL;
?>
--EXPECT--
bool(true)
ValueError: XSLTProcessor::importStylesheet(): Argument #1 ($stylesheet) must not have its clone retained by __clone()
RetainsDocument is still usable: xsl:stylesheet

--TEST--
GH-23730 (Use-after-free when a stylesheet is imported during a transformation)
--EXTENSIONS--
dom
xsl
--CREDITS--
djarfluka
--FILE--
<?php

class MyElement extends DOMElement {
    public function __destruct() {
        /* Runs while the node list of the finished transformation is torn down. */
        import_other('destructor');
    }
}

function import_other(string $from) {
    try {
        $GLOBALS['proc']->importStylesheet($GLOBALS['other']);
        echo $from, ': no error', PHP_EOL;
    } catch (Error $e) {
        echo $from, ': ', $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

function callback($nodes) {
    import_other('callback');
    return $nodes[0];
}

$xml = new DOMDocument();
$xml->registerNodeClass(DOMElement::class, MyElement::class);
$xml->loadXML('<root><item>a</item></root>');

$xsl = new DOMDocument();
$xsl->loadXML(<<<XML
<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl">
  <xsl:template match="/"><xsl:value-of select="php:function('callback', //item)"/></xsl:template>
</xsl:stylesheet>
XML);

$other = new DOMDocument();
$other->loadXML('<?xml version="1.0"?><xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:template match="/">REPLACED</xsl:template></xsl:stylesheet>');

$proc = new XSLTProcessor();
$proc->registerPHPFunctions();
$proc->importStylesheet($xsl);

$GLOBALS['proc'] = $proc;
$GLOBALS['other'] = $other;

var_dump($proc->transformToXml($xml));
var_dump($proc->transformToDoc($xml)->textContent);

$uri = tempnam(sys_get_temp_dir(), 'gh23730');
var_dump($proc->transformToUri($xml, $uri) > 0);
@unlink($uri);

/* Importing outside of a transformation is still allowed. */
var_dump($proc->importStylesheet($other));
var_dump($proc->transformToXml($xml));

?>
--EXPECT--
callback: Error: Cannot call XSLTProcessor::importStylesheet() while a transformation is in progress
destructor: Error: Cannot call XSLTProcessor::importStylesheet() while a transformation is in progress
string(24) "<?xml version="1.0"?>
a
"
callback: Error: Cannot call XSLTProcessor::importStylesheet() while a transformation is in progress
destructor: Error: Cannot call XSLTProcessor::importStylesheet() while a transformation is in progress
string(1) "a"
callback: Error: Cannot call XSLTProcessor::importStylesheet() while a transformation is in progress
destructor: Error: Cannot call XSLTProcessor::importStylesheet() while a transformation is in progress
bool(true)
bool(true)
string(31) "<?xml version="1.0"?>
REPLACED
"

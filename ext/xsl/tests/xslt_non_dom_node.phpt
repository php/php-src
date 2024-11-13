--TEST--
php:function Support - non-DOMNode
--EXTENSIONS--
xsl
--FILE--
<?php
Class foo {
    function __construct() {}
    function __toString() { return "not a DomNode object";}
}

function nonDomNode() {
    return new foo();
}

$dom = new domDocument();
$dom->load(__DIR__."/xslt_non_dom_node.xsl");
$proc = new xsltprocessor;
$xsl = $proc->importStylesheet($dom);

$xml = new DomDocument();
$xml->load(__DIR__."/xslt011.xml");
$proc->registerPHPFunctions();
try {
    $proc->transformToXml($xml);
} catch (TypeError $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Only objects that are instances of DOMNode can be converted to an XPath expression

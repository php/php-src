--TEST--
XSLTProcessor: a php:function() callback cannot modify the document that is being transformed
--EXTENSIONS--
xsl
--FILE--
<?php
function mutate() {
    try {
        ($GLOBALS['mutation'])();
    } catch (DOMException $e) {
        $GLOBALS['errors'][$e::class . ": " . $e->getMessage()] = true;
    }
    return '';
}

function show($nodes) {
    return $nodes[0]->nodeName . ': ' . $nodes[0]->textContent . "\n";
}

function transform(string $label, DOMDocument $doc, string $select, Closure $mutation) {
    $GLOBALS['mutation'] = $mutation;
    $GLOBALS['errors'] = [];
    $xsl = new DOMDocument();
    $xsl->loadXML(<<<XSL
    <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl">
    <xsl:output method="text"/>
    <xsl:template match="/">
    <xsl:for-each select="$select"><xsl:value-of select="php:function('mutate')"/><xsl:value-of select="php:function('show', .)"/></xsl:for-each>
    </xsl:template>
    </xsl:stylesheet>
    XSL);
    $proc = new XSLTProcessor();
    $proc->importStylesheet($xsl);
    $proc->registerPHPFunctions();
    echo "$label:\n", $proc->transformToXml($doc);
    echo implode("\n", array_keys($GLOBALS['errors'])), "\n";
}

$doc = new DOMDocument();
$doc->loadXML('<root a="1" b="2"><a>1</a><a>2</a><a>3</a></root>');
$root = $doc->documentElement;
transform('removeChild', $doc, '/root/a', function () use ($root) {
    foreach (iterator_to_array($root->childNodes) as $child) {
        $root->removeChild($child);
    }
});
transform('textContent', $doc, '/root/a', function () use ($root) {
    $root->textContent = 'x';
});
transform('removeAttribute', $doc, '/root/@*', function () use ($root) {
    $root->removeAttribute('a');
});
echo $doc->saveXML($root), "\n";

$text = new DOMDocument();
$text->loadXML('<root/>');
foreach (['1', '2', '3'] as $data) {
    $text->documentElement->appendChild($text->createTextNode($data));
}
transform('normalize', $text, '/root/text()', function () use ($text) {
    $text->documentElement->normalize();
});
echo $text->saveXML($text->documentElement), "\n";

$other = new DOMDocument();
$other->loadXML('<other><a>1</a><a>2</a><a>3</a></other>');
function other_root() {
    return $GLOBALS['other']->documentElement;
}
transform('foreign document', $doc, "php:function('other_root')/a", function () use ($other) {
    $other->documentElement->firstChild->remove();
});
echo $other->saveXML($other->documentElement), "\n";

$root->firstChild->remove();
$other->documentElement->firstChild->remove();
echo $doc->saveXML($root), "\n", $other->saveXML($other->documentElement), "\n";

function wrap($nodes) {
    $doc = $nodes[0]->ownerDocument;
    $span = $doc->createElement('span');
    $span->setAttribute('class', 'w');
    $span->appendChild($doc->createTextNode(strtoupper($nodes[0]->textContent)));
    return $span;
}
$docless = new DOMElement('e', 'abc');
function docless() {
    return $GLOBALS['docless'];
}
function modify_docless() {
    try {
        $GLOBALS['docless']->textContent = 'xyz';
    } catch (DOMException $e) {
        $GLOBALS['errors'][$e::class . ": " . $e->getMessage()] = true;
    }
    return '';
}
$GLOBALS['errors'] = [];
$xsl = new DOMDocument();
$xsl->loadXML(<<<XSL
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:php="http://php.net/xsl">
<xsl:output method="xml" omit-xml-declaration="yes"/>
<xsl:template match="/">
<out><xsl:for-each select="/root/a"><xsl:copy-of select="php:function('wrap', .)"/></xsl:for-each><xsl:for-each select="php:function('docless')/text()"><xsl:value-of select="php:function('modify_docless')"/><xsl:value-of select="."/></xsl:for-each></out>
</xsl:template>
</xsl:stylesheet>
XSL);
$proc = new XSLTProcessor();
$proc->importStylesheet($xsl);
$proc->registerPHPFunctions();
echo $proc->transformToXml($doc), implode("\n", array_keys($GLOBALS['errors'])), "\n";
?>
--EXPECT--
removeChild:
a: 1
a: 2
a: 3
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
textContent:
a: 1
a: 2
a: 3
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
removeAttribute:
a: 1
b: 2
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
<root a="1" b="2"><a>1</a><a>2</a><a>3</a></root>
normalize:
#text: 1
#text: 2
#text: 3
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
<root>123</root>
foreign document:
a: 1
a: 2
a: 3
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
<other><a>1</a><a>2</a><a>3</a></other>
<root a="1" b="2"><a>2</a><a>3</a></root>
<other><a>2</a><a>3</a></other>
<out xmlns:php="http://php.net/xsl"><span class="w">2</span><span class="w">3</span>abc</out>
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress

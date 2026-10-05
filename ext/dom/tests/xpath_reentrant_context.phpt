--TEST--
DOMXPath: Reentrant evaluation must restore the context node and namespaces
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<r xmlns:p="urn:p"><a><x>A</x><p:y/></a><a><x>A2</x></a><b><x>B</x><x>B2</x><x>B3</x></b></r>');
$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();
$b = $doc->documentElement->lastElementChild;
$GLOBALS['xp'] = $xp;
$GLOBALS['b'] = $b;
function callback($node) {
    $GLOBALS['xp']->query('x[2]', $GLOBALS['b']);
    return true;
}

echo "context node:\n";
var_dump($xp->query('a[php:function("callback", .) and x]', $doc->documentElement)->length);
var_dump($xp->evaluate('count(a[php:function("callback", .) and x])', $doc->documentElement));

echo "in-scope namespace:\n";
var_dump($xp->query('a[php:function("callback", .) and p:y]', $doc->documentElement)->length);

echo "position and size:\n";
var_dump($xp->query('a[php:function("callback", .) and position() = 2]', $doc->documentElement)->length);
var_dump($xp->query('a[php:function("callback", .) and last() = 2]', $doc->documentElement)->length);

echo "after reentry:\n";
var_dump($xp->query('a[x]', $doc->documentElement)->length);

echo "Dom\\XPath:\n";
$modern = Dom\XMLDocument::createFromString('<r><a><x>A</x></a><a><x>A2</x></a><b><x>B</x><x>B2</x><x>B3</x></b></r>');
$GLOBALS['xp'] = new Dom\XPath($modern);
$GLOBALS['xp']->registerNamespace('php', 'http://php.net/xpath');
$GLOBALS['xp']->registerPhpFunctions();
$GLOBALS['b'] = $modern->documentElement->lastElementChild;
var_dump($GLOBALS['xp']->query('a[php:function("callback", .) and x]', $modern->documentElement)->length);
?>
--EXPECT--
context node:
int(2)
float(2)
in-scope namespace:
int(1)
position and size:
int(1)
int(2)
after reentry:
int(2)
Dom\XPath:
int(2)

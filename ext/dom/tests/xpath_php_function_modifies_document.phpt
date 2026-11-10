--TEST--
DOMXPath: a php:function() callback cannot modify the document that is being evaluated
--EXTENSIONS--
dom
--FILE--
<?php
function cb() {
    try {
        ($GLOBALS['mutation'])();
    } catch (DOMException $e) {
        $GLOBALS['errors'][$e::class . ": " . $e->getMessage() . " (" . $e->getCode() . ")"] = true;
    }
    return true;
}

function evaluate(object $xp, string $label, Closure $mutation, string $expr = '/root/node()[php:function("cb")]') {
    $GLOBALS['mutation'] = $mutation;
    $GLOBALS['errors'] = [];
    $res = $xp->evaluate($expr);
    echo $label, ": ", count($res), " nodes\n";
    echo implode("\n", array_keys($GLOBALS['errors'])), "\n";
}

$doc = new DOMDocument();
$doc->loadXML('<root a="1" b="2"><x>1</x><x>2</x><x>3</x></root>');
$root = $doc->documentElement;
$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();

evaluate($xp, 'removeChild', function () use ($root) {
    foreach (iterator_to_array($root->childNodes) as $child) {
        $root->removeChild($child);
    }
});
evaluate($xp, 'replaceChild', function () use ($doc, $root) {
    $root->replaceChild($doc->createElement('y'), $root->lastChild);
});
evaluate($xp, 'textContent', function () use ($root) {
    $root->textContent = 'x';
});
evaluate($xp, 'removeAttribute', function () use ($root) {
    $root->removeAttribute('a');
}, '/root/@*[php:function("cb")]');
echo $doc->saveXML($root), "\n";

$other = new DOMDocument();
$other->loadXML('<other><y/></other>');
evaluate($xp, 'other document', function () use ($other) {
    $other->documentElement->firstChild?->remove();
});
echo $other->saveXML($other->documentElement), "\n";

function build($nodes) {
    $doc = $nodes[0]->ownerDocument;
    $span = $doc->createElement('span');
    $span->setAttribute('class', 'w');
    $span->appendChild($doc->createTextNode(strtoupper($nodes[0]->textContent)));
    $doc->formatOutput = true;
    $doc->documentURI = 'urn:built';
    try {
        $doc->documentElement->appendChild($doc->createElement('extra'));
    } catch (DOMException $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    return $span;
}
foreach ($xp->query('php:function("build", /root/x[1])') as $node) {
    echo $doc->saveXML($node), " ", $doc->documentURI, "\n";
}
$doc->formatOutput = false;

$doc->strictErrorChecking = false;
$GLOBALS['mutation'] = function () use ($root) {
    var_dump($root->removeChild($root->firstChild));
};
var_dump($xp->evaluate('boolean(/root[php:function("cb")])'));
$doc->strictErrorChecking = true;

$root->removeChild($root->firstChild);
$root->removeAttribute('a');
echo $doc->saveXML($root), "\n";

$modern = Dom\XMLDocument::createFromString('<root><x>1</x><x>2</x></root>');
$modern_xp = new Dom\XPath($modern);
$modern_xp->registerNamespace('php', 'http://php.net/xpath');
$modern_xp->registerPhpFunctions();
evaluate($modern_xp, 'Dom\Element::remove', function () use ($modern) {
    $modern->documentElement->firstChild->remove();
});
evaluate($modern_xp, 'Dom\Element::$innerHTML', function () use ($modern) {
    $modern->documentElement->innerHTML = '<y/>';
});
echo $modern->saveXml($modern->documentElement), "\n";
?>
--EXPECTF--
removeChild: 3 nodes
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress (7)
replaceChild: 3 nodes
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress (7)
textContent: 3 nodes
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress (7)
removeAttribute: 2 nodes
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress (7)
<root a="1" b="2"><x>1</x><x>2</x><x>3</x></root>
other document: 3 nodes

<other/>
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
<span class="w">1</span> urn:built

Warning: DOMNode::removeChild(): Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress in %s on line %d
bool(false)
bool(true)
<root b="2"><x>2</x><x>3</x></root>
Dom\Element::remove: 2 nodes
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress (7)
Dom\Element::$innerHTML: 2 nodes
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress (7)
<root><x>1</x><x>2</x></root>

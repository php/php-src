--TEST--
GH-9628 (Implicitly removing nodes from \DOMDocument breaks existing references) - simple variation
--EXTENSIONS--
dom
--FILE--
<?php
$html = <<<'HTML'
<p xmlns="some:ns">
    <span id="1" xmlns:test="some:ns2">
        <strong>
            <span id="2">Test <test:test/></span>
        </strong>
    </span>
</p>
HTML;

$doc = new DOMDocument('1.0', 'UTF-8');
$doc->loadXML($html);

$outer_span = $doc->documentElement->firstElementChild;
$inner_span = $outer_span->firstElementChild->firstElementChild;
var_dump($inner_span->namespaceURI);

// Remove strong
$outer_span->firstElementChild->remove();

var_dump($inner_span->getAttribute('id'));
var_dump($inner_span->namespaceURI);

// Import test
$doc = new DOMDocument();
$doc->append($doc->importNode($outer_span, true), $doc->importNode($inner_span, true));
echo $doc->saveXML();

var_dump($inner_span->getAttribute('id'));
var_dump($inner_span->namespaceURI);
?>
--EXPECTF--
string(7) "some:ns"

Fatal error: Uncaught Error: Couldn't fetch DOMElement in %s:%d
Stack trace:
#0 %s(%d): DOMElement->getAttribute('id')
#1 {main}
  thrown in %s on line %d

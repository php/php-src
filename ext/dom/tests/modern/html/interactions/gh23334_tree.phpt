--TEST--
GH-23334 (template contents must not be part of the host element's tree)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = Dom\HTMLDocument::createFromString(
    '<!doctype html><body><div id="host"><template id="tpl"><b id="inner">X</b></template></div>',
    LIBXML_NOERROR
);
$tpl = $doc->getElementById('tpl');
$inner = $doc->getElementById('inner');
$frag = $inner->parentNode;

var_dump($frag::class);
var_dump($frag->parentNode);
var_dump($frag->parentElement);
var_dump($frag->getRootNode() === $frag);
var_dump($inner->getRootNode() === $frag);
var_dump($frag->isConnected);
var_dump($inner->isConnected);

var_dump($frag->contains($inner));
var_dump($tpl->contains($inner));
var_dump($doc->body->contains($inner));

var_dump($inner->closest('b')?->nodeName);
var_dump($inner->closest('#host'));
var_dump($inner->closest('body'));

$pos = $tpl->compareDocumentPosition($inner);
var_dump((bool) ($pos & 1));
var_dump((bool) ($pos & 0x10));

try {
    $frag->appendChild($tpl);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
string(20) "Dom\DocumentFragment"
NULL
NULL
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
string(1) "B"
NULL
NULL
bool(true)
bool(false)
DOMException: Hierarchy Request Error

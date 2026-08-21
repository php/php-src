--TEST--
GH-23334 (UAF when template contents are freed while userland references remain)
--EXTENSIONS--
dom
--FILE--
<?php
$html = '<!doctype html><body><div id="host"><template id="tpl"><div id="inner">HELLO</div></template></div>';

// A descendant of the template contents outlives the template element.
$doc = Dom\HTMLDocument::createFromString($html, LIBXML_NOERROR);
$inner = $doc->getElementById('inner');
$tpl = $doc->getElementById('tpl');
$tpl->remove();
unset($tpl);
var_dump($inner->textContent);
var_dump($inner->parentNode);

// The template contents fragment itself outlives the template element.
$doc = Dom\HTMLDocument::createFromString($html, LIBXML_NOERROR);
$frag = $doc->getElementById('inner')->parentNode;
var_dump($frag::class);
$doc->getElementById('host')->innerHTML = '';
var_dump($frag->childNodes->length);
?>
--EXPECT--
string(5) "HELLO"
NULL
string(20) "Dom\DocumentFragment"
int(0)

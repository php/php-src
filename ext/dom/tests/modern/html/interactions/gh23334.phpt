--TEST--
GH-23334 (Template contents must not be reachable through the ID cache)
--EXTENSIONS--
dom
--FILE--
<?php
$html = '<!doctype html><body><div id="host"><template id="tpl"><div id="inner">HELLO</div></template></div>';

$doc = Dom\HTMLDocument::createFromString($html, LIBXML_NOERROR);
var_dump($doc->getElementById('inner'));
var_dump((new Dom\XPath($doc))->query("id('inner')")->length);
var_dump($doc->getElementById('tpl')->innerHTML);

$doc = Dom\HTMLDocument::createFromString($html, LIBXML_NOERROR);
$tpl = $doc->getElementById('tpl');
$tpl->innerHTML = '<p id="set">WORLD</p>';
var_dump((new Dom\XPath($doc))->query("id('set')")->length);
var_dump($tpl->innerHTML);
$doc->getElementById('host')->innerHTML = '';
?>
--EXPECT--
NULL
int(0)
string(27) "<div id="inner">HELLO</div>"
int(0)
string(21) "<p id="set">WORLD</p>"

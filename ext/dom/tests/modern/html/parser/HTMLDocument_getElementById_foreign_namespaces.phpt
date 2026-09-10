--TEST--
Dom\HTMLDocument::getElementById() finds ids of SVG and MathML elements
--EXTENSIONS--
dom
--FILE--
<?php
$html = '<!DOCTYPE html><html><body><svg id="s"><rect xml:id="r"/></svg><math id="m"></math><p id="p"></p></body></html>';
$d = Dom\HTMLDocument::createFromString($html, LIBXML_NOERROR);
var_dump([
    'svg #s' => $d->getElementById('s')?->tagName,
    'math #m' => $d->getElementById('m')?->tagName,
    'html #p' => $d->getElementById('p')?->tagName,
    'xml:id #r' => $d->getElementById('r')?->tagName,
]);

/* A fragment parsed into a foreign context inherits the context namespace, so
 * this id really is namespaced and must not be registered. */
$d2 = Dom\HTMLDocument::createEmpty();
$z = $d2->createElementNS('http://www.w3.org/1999/xlink', 'z');
$d2->appendChild($z);
$z->innerHTML = '<z id="q"></z>';
$attr = $z->firstChild->attributes->item(0);
var_dump($attr->localName, $attr->namespaceURI, $d2->getElementById('q'));
?>
--EXPECT--
array(4) {
  ["svg #s"]=>
  string(3) "svg"
  ["math #m"]=>
  string(4) "math"
  ["html #p"]=>
  string(1) "P"
  ["xml:id #r"]=>
  NULL
}
string(2) "id"
string(28) "http://www.w3.org/1999/xlink"
NULL

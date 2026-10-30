--TEST--
Unprefixed attributes in foreign content are in no namespace
--EXTENSIONS--
dom
--FILE--
<?php
$html = '<!DOCTYPE html><html><body><svg id="s" xlink:href="#a" xml:lang="en" xmlns:xlink="urn:x"><rect xml:id="r"/></svg><math id="m"></math><p id="p"></p></body></html>';
$doc = Dom\HTMLDocument::createFromString($html, LIBXML_NOERROR);

var_dump([
    'svg #s' => $doc->getElementById('s')?->tagName,
    'math #m' => $doc->getElementById('m')?->tagName,
    'html #p' => $doc->getElementById('p')?->tagName,
    'xml:id #r' => $doc->getElementById('r')?->tagName,
]);

foreach ($doc->getElementById('s')->attributes as $attr) {
    echo $attr->name, ' => ', var_export($attr->namespaceURI, true), "\n";
}

$contexts = [
    'http://www.w3.org/1999/xlink' => 'z',
    'http://www.w3.org/XML/1998/namespace' => 'z',
    'http://www.w3.org/2000/xmlns/' => 'xmlns',
];
foreach ($contexts as $uri => $name) {
    $fragment_doc = Dom\HTMLDocument::createEmpty();
    $context = $fragment_doc->createElementNS($uri, $name);
    $fragment_doc->appendChild($context);
    $context->innerHTML = '<z id="q" xlink:href="#a"></z>';

    echo $uri, "\n  ", $context->innerHTML, "\n  ";
    var_dump($fragment_doc->getElementById('q')?->tagName);
}
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
id => NULL
xlink:href => 'http://www.w3.org/1999/xlink'
xml:lang => 'http://www.w3.org/XML/1998/namespace'
xmlns:xlink => 'http://www.w3.org/2000/xmlns/'
http://www.w3.org/1999/xlink
  <z id="q" xlink:href="#a"></z>
  string(1) "Z"
http://www.w3.org/XML/1998/namespace
  <z id="q" xlink:href="#a"></z>
  string(1) "Z"
http://www.w3.org/2000/xmlns/
  <z id="q" xlink:href="#a"></z>
  string(1) "Z"

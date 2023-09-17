--TEST--
DOM\HTMLDocument serialization of void elements
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();

$tags = [
    "area",
    "base",
    "br",
    "col",
    "embed",
    "hr",
    "img",
    "input",
    "link",
    "meta",
    "source",
    "track",
    "wbr",
    "basefont",
    "bgsound",
    "frame",
    "keygen",
    "param",
];

foreach ($tags as $tag) {
    $element = $dom->appendChild($dom->createElement($tag));
    $element->appendChild($dom->createElement("inner"));
    $element->after("\n");
    echo "$tag: ";
    var_dump(strlen($dom->saveHTML($element)) === 0);

    $element = $dom->appendChild($dom->createElementNS("http://php.net/foo", "x:$tag"));
    $element->appendChild($dom->createElement("inner"));
    $element->after("\n");
}

echo $dom->saveHTML();

?>
--EXPECT--
area: bool(true)
base: bool(true)
br: bool(true)
col: bool(true)
embed: bool(true)
hr: bool(true)
img: bool(true)
input: bool(true)
link: bool(true)
meta: bool(true)
source: bool(true)
track: bool(true)
wbr: bool(true)
basefont: bool(true)
bgsound: bool(true)
frame: bool(true)
keygen: bool(true)
param: bool(true)
<area>
<x:area><inner></inner></x:area>
<base>
<x:base><inner></inner></x:base>
<br>
<x:br><inner></inner></x:br>
<col>
<x:col><inner></inner></x:col>
<embed>
<x:embed><inner></inner></x:embed>
<hr>
<x:hr><inner></inner></x:hr>
<img>
<x:img><inner></inner></x:img>
<input>
<x:input><inner></inner></x:input>
<link>
<x:link><inner></inner></x:link>
<meta>
<x:meta><inner></inner></x:meta>
<source>
<x:source><inner></inner></x:source>
<track>
<x:track><inner></inner></x:track>
<wbr>
<x:wbr><inner></inner></x:wbr>
<basefont>
<x:basefont><inner></inner></x:basefont>
<bgsound>
<x:bgsound><inner></inner></x:bgsound>
<frame>
<x:frame><inner></inner></x:frame>
<keygen>
<x:keygen><inner></inner></x:keygen>
<param>
<x:param><inner></inner></x:param>

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
    var_dump($dom->saveHTML($element));

    $element = $dom->appendChild($dom->createElementNS("http://php.net/foo", "x:$tag"));
    $element->appendChild($dom->createElement("inner"));
    $element->after("\n");
}

echo $dom->saveHTML();

?>
--EXPECT--
area: string(6) "<area>"
base: string(6) "<base>"
br: string(4) "<br>"
col: string(5) "<col>"
embed: string(7) "<embed>"
hr: string(4) "<hr>"
img: string(5) "<img>"
input: string(7) "<input>"
link: string(6) "<link>"
meta: string(6) "<meta>"
source: string(8) "<source>"
track: string(7) "<track>"
wbr: string(5) "<wbr>"
basefont: string(10) "<basefont>"
bgsound: string(9) "<bgsound>"
frame: string(7) "<frame>"
keygen: string(8) "<keygen>"
param: string(7) "<param>"
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

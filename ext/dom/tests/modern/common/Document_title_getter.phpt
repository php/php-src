--TEST--
Dom\Document::$title getter
--EXTENSIONS--
dom
--FILE--
<?php

echo "=== HTML namespaced root ===\n";

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title>A normal title without collapsable or strippable whitespace</title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title>  only ws at front</title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title>only ws at back  </title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><ignoreme/><div><title>first</title></div><title>second</title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<title xmlns=\"http://www.w3.org/1999/xhtml\">title</title>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title>  abc def   ghi  </title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title></title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title>  \t\r\n  </title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title>  \tx<?y y?><![CDATA[z]]>\n  </title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title><div><!-- comment -->x</div>y<p>z</p>w</title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title>title\nhere</title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title/></root>");
$dom->getElementsByTagName('title')[0]->appendChild($dom->importLegacyNode(new DOMText));
var_dump($dom->title);

echo "=== SVG namespaced root ===\n";

$dom = Dom\XMLDocument::createFromString("<root xmlns=\"http://www.w3.org/1999/xhtml\"><title>title</title></root>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<svg xmlns=\"http://www.w3.org/1999/xhtml\"><title xmlns=\"http://www.w3.org/1999/xhtml\">title</title></svg>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<svg xmlns=\"http://www.w3.org/1999/xhtml\"><title xmlns=\"http://www.w3.org/1999/xhtml\">title</title><foo/><title>hi</title></svg>");
var_dump($dom->title);

$dom = Dom\XMLDocument::createFromString("<svg xmlns=\"http://www.w3.org/1999/xhtml\"/>");
var_dump($dom->title);

?>
--EXPECT--
=== HTML namespaced root ===
string(59) "A normal title without collapsable or strippable whitespace"
string(16) "only ws at front"
string(15) "only ws at back"
string(5) "first"
string(5) "title"
string(11) "abc def ghi"
string(0) ""
string(0) ""
string(0) ""
string(2) "xz"
string(2) "yw"
string(10) "title here"
string(0) ""
=== SVG namespaced root ===
string(5) "title"
string(5) "title"
string(5) "title"
string(0) ""

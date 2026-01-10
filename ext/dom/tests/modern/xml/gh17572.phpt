--TEST--
GH-17572 (getElementsByTagName returns collections with tagName-based indexing, causing loss of elements when converted to arrays)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString(<<<XML
<!DOCTYPE root [
    <!ENTITY a "a">
]>
<root>
    <p/>
    <p xmlns:x="urn:x" x:a="1" b="2" a="3"/>
</root>
XML);

echo "--- querySelectorAll('p') ---\n";

foreach ($dom->querySelectorAll('p') as $k => $v) {
    var_dump($k, $v->nodeName);
}

echo "--- getElementsByTagName('p') ---\n";

foreach ($dom->getElementsByTagName('p') as $k => $v) {
    var_dump($k, $v->nodeName);
}

echo "--- entities ---\n";

foreach ($dom->doctype->entities as $k => $v) {
    var_dump($k, $v->nodeName);
}

echo "--- attributes ---\n";

$attribs = $dom->getElementsByTagName('p')[1]->attributes;
foreach ($attribs as $k => $v) {
    var_dump($k, $v->value);
    var_dump($attribs[$k]->value);
}

?>
--EXPECT--
--- querySelectorAll('p') ---
int(0)
string(1) "p"
int(1)
string(1) "p"
--- getElementsByTagName('p') ---
int(0)
string(1) "p"
int(1)
string(1) "p"
--- entities ---
string(1) "a"
string(1) "a"
--- attributes ---
string(7) "xmlns:x"
string(5) "urn:x"
string(5) "urn:x"
string(3) "x:a"
string(1) "1"
string(1) "1"
string(1) "b"
string(1) "2"
string(1) "2"
string(1) "a"
string(1) "3"
string(1) "3"

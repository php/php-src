--TEST--
Document::createAttribute()
--EXTENSIONS--
dom
--FILE--
<?php
function dumpAttr(DOM\Attr $attr) {
    echo "Attr: {$attr->name}\n";
    var_dump($attr->prefix);
    var_dump($attr->nodeName);
    var_dump($attr->namespaceURI);
}

echo "--- HTML document ---\n";

$dom = DOM\HTMLDocument::createEmpty();
$attr = $dom->createAttribute('foo');
dumpAttr($attr);
$attr = $dom->createAttribute('FoOo');
dumpAttr($attr);

echo "--- XML document ---\n";

$dom = DOM\XMLDocument::createEmpty();
$attr = $dom->createAttribute('foo');
dumpAttr($attr);
$attr = $dom->createAttribute('FoOo');
dumpAttr($attr);
?>
--EXPECT--
--- HTML document ---
Attr: foo
string(0) ""
string(3) "foo"
NULL
Attr: fooo
string(0) ""
string(4) "fooo"
NULL
--- XML document ---
Attr: foo
string(0) ""
string(3) "foo"
NULL
Attr: FoOo
string(0) ""
string(4) "FoOo"
NULL

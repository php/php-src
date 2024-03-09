--TEST--
Document::createAttribute()
--EXTENSIONS--
dom
--FILE--
<?php
require __DIR__ . "/dump_attr.inc";

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
NULL
string(3) "foo"
NULL
Attr: fooo
NULL
string(4) "fooo"
NULL
--- XML document ---
Attr: foo
NULL
string(3) "foo"
NULL
Attr: FoOo
NULL
string(4) "FoOo"
NULL

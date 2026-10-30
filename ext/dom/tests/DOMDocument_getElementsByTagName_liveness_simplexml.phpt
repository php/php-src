--TEST--
DOMDocument::getElementsByTagName() liveness with simplexml_import_dom
--EXTENSIONS--
dom
simplexml
--FILE--
<?php

$doc = new DOMDocument;
$doc->loadXML( '<root><e i="1"/><e i="2"/><e i="3"/><e i="4"/><e i="5"/><e i="6"/><e i="7"/><e i="8"/><e i="9"/><e i="10"/></root>' );
$list = $doc->getElementsByTagName('e');
print $list->item(5)->getAttribute('i')."\n";
echo "before import\n";
$s = simplexml_import_dom($doc->documentElement);
echo "after import\n";

unset($s->e[5]);
print $list->item(5)->getAttribute('i')."\n";

unset($s->e[5]);
print $list->item(5)->getAttribute('i')."\n";

?>
--EXPECT--
6
before import
after import
7
8

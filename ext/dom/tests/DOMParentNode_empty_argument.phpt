--TEST--
DOMParentNode functions with empty argument
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument();
$dom->loadXML('<?xml version="1.0"?><root><node/></root>');

$emptyFragment = $dom->createDocumentFragment();

echo "--- DOMElement test ---\n";

$dom->documentElement->after(...$emptyFragment->childNodes);
$dom->documentElement->before(...$emptyFragment->childNodes);
$dom->documentElement->append(...$emptyFragment->childNodes);
$dom->documentElement->prepend(...$emptyFragment->childNodes);
$dom->documentElement->after();
$dom->documentElement->before();
$dom->documentElement->append();
$dom->documentElement->prepend();
echo $dom->saveXML();

$dom->documentElement->firstChild->replaceWith(...$emptyFragment->childNodes);
echo $dom->saveXML();

$dom->documentElement->replaceWith(...$emptyFragment->childNodes);
echo $dom->saveXML();

echo "--- DOMDocumentFragment test ---\n";

$fragment = $dom->createDocumentFragment();
$fragment->append($dom->createElement('foo'));
$fragment->append(...$emptyFragment->childNodes);
$fragment->prepend(...$emptyFragment->childNodes);
$fragment->append();
$fragment->prepend();
echo $dom->saveXML($fragment), "\n";

echo "--- DOMDocument test ---\n";

$dom->append(...$emptyFragment->childNodes);
$dom->prepend(...$emptyFragment->childNodes);
$dom->append();
$dom->prepend();
echo $dom->saveXML(), "\n";

echo "--- DOMCharacterData test ---\n";

$cdata = $dom->createCDATASection('foo');
$dom->appendChild($cdata);

$cdata->after(...$emptyFragment->childNodes);
$cdata->before(...$emptyFragment->childNodes);
$cdata->after();
$cdata->before();
echo $dom->saveXML(), "\n";
$cdata->replaceWith(...$emptyFragment->childNodes);
echo $dom->saveXML(), "\n";

$cdata = $dom->createCDATASection('foo');
$dom->appendChild($cdata);
$cdata->replaceWith(...$emptyFragment->childNodes);
echo $dom->saveXML(), "\n";
?>
--EXPECT--
--- DOMElement test ---
<?xml version="1.0"?>
<root><node/></root>
<?xml version="1.0"?>
<root/>
<?xml version="1.0"?>
--- DOMDocumentFragment test ---
<foo/>
--- DOMDocument test ---
<?xml version="1.0"?>

--- DOMCharacterData test ---
<?xml version="1.0"?>
<![CDATA[foo]]>

<?xml version="1.0"?>

<?xml version="1.0"?>

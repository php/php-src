--TEST--
Dom\Document::$title setter
--EXTENSIONS--
dom
--FILE--
<?php

echo "\n=== SVG namespaced test ===\n\n";

$dom = Dom\XMLDocument::createFromString('<root xmlns="http://www.w3.org/2000/svg"/>');
$dom->title = "hello &amp; world";
echo $dom->saveXML(), "\n";

$dom = Dom\XMLDocument::createFromString('<svg xmlns="http://www.w3.org/2000/svg"/>');
$dom->title = "hello &amp; world";
echo $dom->saveXML(), "\n";
$dom->title = "";
echo $dom->saveXML(), "\n";
$dom->title = "test";
echo $dom->saveXML(), "\n";

$dom = Dom\XMLDocument::createFromString('<svg:svg xmlns:svg="http://www.w3.org/2000/svg"/>');
$dom->title = "test";
echo $dom->saveXML(), "\n";
var_dump($dom->documentElement->firstElementChild->prefix, $dom->documentElement->firstElementChild->namespaceURI);

$dom = Dom\XMLDocument::createFromString('<svg xmlns="http://www.w3.org/2000/svg">first node<div/></svg>');
$dom->title = "test";
echo $dom->saveXML(), "\n";
$dom->documentElement->firstElementChild->remove();
$dom->title = "test2";
echo $dom->saveXML(), "\n";

echo "\n=== HTML namespaced test ===\n\n";

$dom = Dom\XMLDocument::createFromString('<root xmlns="http://www.w3.org/1999/xhtml"/>');
$dom->title = "test";
echo $dom->saveXML(), "\n";

$dom = Dom\XMLDocument::createFromString('<html xmlns="http://www.w3.org/1999/xhtml"/>');
$dom->title = "test";
echo $dom->saveXML(), "\n";

$dom = Dom\XMLDocument::createFromString('<html xmlns="http://www.w3.org/1999/xhtml"><foo/><head/></html>');
$dom->title = "test";
echo $dom->saveXML(), "\n";

$dom = Dom\XMLDocument::createFromString('<html xmlns="http://www.w3.org/1999/xhtml"><head><?ignore me?></head></html>');
$dom->title = "test";
echo $dom->saveXML(), "\n";

$dom = Dom\XMLDocument::createFromString('<html xmlns="http://www.w3.org/1999/xhtml"><head><?ignore me?><title>foo<div/></title></head></html>');
$dom->title = "test";
echo $dom->saveXML(), "\n";

echo "\n=== neither namespaced test ===\n\n";

$dom = Dom\XMLDocument::createEmpty();
$dom->title = "";
echo $dom->saveXML(), "\n";

$dom = Dom\XMLDocument::createFromString('<root/>');
$dom->title = "test";
echo $dom->saveXML(), "\n";

?>
--EXPECT--
=== SVG namespaced test ===

<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="http://www.w3.org/2000/svg"/>
<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg"><title>hello &amp;amp; world</title></svg>
<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg"><title></title></svg>
<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg"><title>test</title></svg>
<?xml version="1.0" encoding="UTF-8"?>
<svg:svg xmlns:svg="http://www.w3.org/2000/svg"><svg:title>test</svg:title></svg:svg>
NULL
string(26) "http://www.w3.org/2000/svg"
<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg"><title>test</title>first node<div/></svg>
<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg"><title>test2</title>first node<div/></svg>

=== HTML namespaced test ===

<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="http://www.w3.org/1999/xhtml"></root>
<?xml version="1.0" encoding="UTF-8"?>
<html xmlns="http://www.w3.org/1999/xhtml"></html>
<?xml version="1.0" encoding="UTF-8"?>
<html xmlns="http://www.w3.org/1999/xhtml"><foo></foo><head><title>test</title></head></html>
<?xml version="1.0" encoding="UTF-8"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head><?ignore me?><title>test</title></head></html>
<?xml version="1.0" encoding="UTF-8"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head><?ignore me?><title>test</title></head></html>

=== neither namespaced test ===

<?xml version="1.0" encoding="UTF-8"?>

<?xml version="1.0" encoding="UTF-8"?>
<root/>

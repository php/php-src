--TEST--
Test writing Element::$outerHTML on HTML documents
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);
$p = $dom->body->firstChild;
$p->outerHTML = '<div></div>&nbsp;<p>'; // intentionally unclosed
echo $dom->saveXML(), "\n";
echo $dom->saveHtml(), "\n";
$div = $dom->body->firstChild;
$div->outerHTML = "invalid\xffutf-8𐍈𐍈𐍈";
echo $dom->saveXML(), "\n";
echo $dom->saveHtml(), "\n";

$dom->body->outerHTML = '<template><p>foo</p></template>';
var_dump($dom->body->querySelector('p')); // Should be NULL because the template contents do not participate in the DOM tree
echo $dom->saveXML(), "\n";
echo $dom->saveHtml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div></div> <p></p></body></html>
<html><head></head><body><div></div>&nbsp;<p></p></body></html>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body>invalid�utf-8𐍈𐍈𐍈 <p></p></body></html>
<html><head></head><body>invalid�utf-8𐍈𐍈𐍈&nbsp;<p></p></body></html>
NULL
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><head><template><p>foo</p></template></head><body></body></html>
<html><head></head><head><template><p>foo</p></template></head><body></body></html>

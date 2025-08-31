--TEST--
Test writing Element::$innerHTML on HTML documents
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$el = $dom->createElement('div');
$dom->appendChild($el);
$el->innerHTML = '<p>foo</p>';
echo $dom->saveXML(), "\n";
$el->innerHTML = '</div>';
echo $dom->saveXML(), "\n";
$el->innerHTML = '';
echo $dom->saveXML(), "\n";
$el->innerHTML = '<div></div>&nbsp;<p></p>';
echo $dom->saveXML(), "\n";
$el->innerHTML = "invalid\xffutf-8𐍈𐍈𐍈";
echo $dom->saveXML(), "\n";

// Create a non-interned string that gets the UTF-8 validity flag added
$str = str_repeat("my valid string", random_int(1, 1));
preg_match('/^.*$/u', $str);
$el->innerHTML = $str;
echo $dom->saveXML(), "\n";

$dom = DOM\HTMLDocument::createEmpty();
$el = $dom->createElement('style');
$dom->appendChild($el);
$el->innerHTML = '<p>foo</p>';
echo $dom->saveXML(), "\n";

$dom = DOM\HTMLDocument::createEmpty();
$el = $dom->createElementNS('urn:a', 'style');
$dom->appendChild($el);
$el->innerHTML = '<p>foo</p>';
echo $dom->saveXML(), "\n";

$dom = DOM\HTMLDocument::createEmpty();
$el = $dom->createElement('textarea');
$dom->appendChild($el);
$el->innerHTML = "</textarea>\0-->";
echo $dom->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<div xmlns="http://www.w3.org/1999/xhtml"><p>foo</p></div>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<div xmlns="http://www.w3.org/1999/xhtml"></div>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<div xmlns="http://www.w3.org/1999/xhtml"></div>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<div xmlns="http://www.w3.org/1999/xhtml"><div></div> <p></p></div>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<div xmlns="http://www.w3.org/1999/xhtml">invalid�utf-8𐍈𐍈𐍈</div>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<div xmlns="http://www.w3.org/1999/xhtml">my valid string</div>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<style xmlns="http://www.w3.org/1999/xhtml">&lt;p&gt;foo&lt;/p&gt;</style>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<style xmlns="urn:a"><p xmlns="">foo</p></style>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<textarea xmlns="http://www.w3.org/1999/xhtml">&lt;/textarea&gt;�--&gt;</textarea>

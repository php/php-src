--TEST--
GH-15670 (Polymorphic cache slot issue in DOM)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadHTML('<p id=x>foo</p>');
$dom = DOM\XMLDocument::createFromString('<root/>');
$child = $dom->documentElement->appendChild($dom->createElementNS('urn:a', 'child'));
function test($child, $html) {
    try {
        $child->innerHTML = $html;
    } catch (DOMException $e) {
    }
}
test($child, '--></root><!--');
test($doc, '<');
echo $doc->saveXML(), "\n";
echo $dom->saveXML(), "\n";
?>
--EXPECTF--
Deprecated: Creation of dynamic property DOMDocument::$innerHTML is deprecated in %s on line %d
<?xml version="1.0" standalone="yes"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">
<html><body><p id="x">foo</p></body></html>

<?xml version="1.0" encoding="UTF-8"?>
<root><child xmlns="urn:a"/></root>

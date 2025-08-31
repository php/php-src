--TEST--
GH-9142 (DOMChildNode replaceWith() double-free error when replacing elements not separated by any whitespace)
--EXTENSIONS--
dom
--FILE--
<?php

$document = '<var>One</var><var>Two</var>';

($dom = new DOMDocument('1.0', 'UTF-8'))->loadHTML($document);

foreach ((new DOMXPath($dom))->query('//var') as $var) {
    $var->replaceWith($dom->createElement('p', $var->nodeValue));
}

var_dump($dom->saveHTML());

?>
--EXPECT--
string(154) "<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">
<html><body><p>One</p><p>Two</p></body></html>
"

--TEST--
GH-20395 (\Dom\ParentNode::querySelector and \Dom\ParentNode::querySelectorAll requires elements in $selectors to be lowercase)
--EXTENSIONS--
dom
--CREDITS--
DeveloperRob
--FILE--
<?php

$html   = '<!doctype html><html><head></head><body></body></html>';
$dom    = Dom\HtmlDocument::createFromString($html);
var_dump(is_null($dom->querySelector('html')));
var_dump(is_null($dom->querySelector('Html')));
var_dump(is_null($dom->querySelector('HTML')));

$dom->body->appendChild($dom->createElement('div'));
$dom->body->appendChild($dom->createElementNS('http://www.w3.org/1999/xhtml', 'Div'));

foreach ($dom->querySelectorAll('div') as $div) {
	var_dump($div->localName);
}

foreach ($dom->querySelectorAll('Div') as $div) {
	var_dump($div->localName);
}

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
string(3) "div"
string(3) "div"

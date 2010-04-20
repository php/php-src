--TEST--
Bug #51615 (PHP crash with wrong HTML in SimpleXML)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadHTML('<span title=""y">x</span><span title=""z">x</span>');
$html = simplexml_import_dom($dom);

foreach ($html->body->span as $obj) {
	var_dump((string)$obj->title);
}

?>
--EXPECTF--
Warning: DOMDocument::loadHTML(): error parsing attribute name in Entity, line: 1 in %s on line %d

Warning: DOMDocument::loadHTML(): error parsing attribute name in Entity, line: 1 in %s on line %d
string(0) ""
string(0) ""

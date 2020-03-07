--TEST--
Bug #51615 (PHP crash with wrong HTML in SimpleXML)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip";
 if (!extension_loaded("dom")) print "skip";
?>
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadHTML('<span title=""y">x</span><span title=""z">x</span>');
$html = simplexml_import_dom($dom);

var_dump($html->body->span);

foreach ($html->body->span as $obj) {
	var_dump((string)$obj->title);
}

?>
--EXPECTF--
Warning: DOMDocument::loadHTML(): error parsing attribute name in Entity, line: 1 in %s on line %d

Warning: DOMDocument::loadHTML(): error parsing attribute name in Entity, line: 1 in %s on line %d
object(SimpleXMLElement)#5 (3) {
  ["@attributes"]=>
  array(2) {
    ["title"]=>
    string(0) ""
    ["y"]=>
    string(0) ""
  }
  [0]=>
  object(SimpleXMLElement)#4 (2) {
    ["@attributes"]=>
    array(2) {
      ["title"]=>
      string(0) ""
      ["y"]=>
      string(0) ""
    }
    [0]=>
    string(1) "x"
  }
  [1]=>
  object(SimpleXMLElement)#6 (2) {
    ["@attributes"]=>
    array(2) {
      ["title"]=>
      string(0) ""
      ["z"]=>
      string(0) ""
    }
    [0]=>
    string(1) "x"
  }
}
string(0) ""
string(0) ""

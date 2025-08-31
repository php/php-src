--TEST--
Bug #51615 (PHP crash with wrong HTML in SimpleXML)
--EXTENSIONS--
simplexml
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadHTML('<span title=""y">x</span><span title=""z">x</span>', LIBXML_NOERROR);
$html = simplexml_import_dom($dom);

var_dump($html->body->span);

foreach ($html->body->span as $obj) {
    var_dump((string)$obj->title);
}

?>
--EXPECTF--
object(SimpleXMLElement)#%d (3) {
  ["@attributes"]=>
  array(2) {
    ["title"]=>
    string(0) ""
    [%r("y"{1,2})%r]=>
    string(0) ""
  }
  [0]=>
  string(1) "x"
  [1]=>
  string(1) "x"
}
string(0) ""
string(0) ""

--TEST--
Bug #42082 (NodeList length zero should be empty)
--FILE--
<?php
$doc = new DOMDocument();
$xpath = new DOMXPath($doc);
$nodes = $xpath->query('*');
var_dump($nodes);
var_dump($nodes->length);
$length = $nodes->length;
var_dump(empty($nodes->length), empty($lenght));

$doc->loadXML("<element></element>");
var_dump($doc->firstChild->nodeValue, empty($doc->firstChild->nodeValue), isset($doc->firstChild->nodeValue));
var_dump(empty($doc->nodeType), empty($doc->firstChild->nodeType))
?>
--EXPECTF--
object(DOMNodeList)#%d (0) {
}
int(0)
bool(true)
bool(true)
string(0) ""
bool(true)
bool(true)
bool(false)
bool(false)

--TEST--
Bug #47430 (Errors after writing to nodeValue parameter of an absent previousSibling).
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$xml = '<?xml
version="1.0"?><html><p><i>Hello</i></p><p><i>World!</i></p></html>';
$dom = new DOMDocument();
$dom->loadXML($xml);

$elements = $dom->getElementsByTagName('i');
foreach ($elements as $i) {
  $i->previousSibling->nodeValue = '';
}

$arr = array();
$arr[0] = 'Value';

print_r($arr);

?>
--EXPECTF--
Warning: Attempt to assign property 'nodeValue' of non-object in %s on line %d

Warning: Attempt to assign property 'nodeValue' of non-object in %s on line %d
Array
(
    [0] => Value
)

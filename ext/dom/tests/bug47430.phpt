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
    try {
        $i->previousSibling->nodeValue = '';
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

$arr = array();
$arr[0] = 'Value';

print_r($arr);

?>
--EXPECT--
Attempt to assign property "nodeValue" on null
Attempt to assign property "nodeValue" on null
Array
(
    [0] => Value
)

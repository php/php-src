--TEST--
Test iterator interaction with empty and var_dump
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = <<<XML
<container>
    <first><foo/></first>
</container>
XML;

$sxe = simplexml_load_string($xml);

echo "--- var_dump ---\n";

$first = $sxe->first;
$first->rewind();
var_dump($first->current()->getName());
var_dump($first);
var_dump($first->current()->getName());

echo "--- empty ---\n";

$first = $sxe->first;
$first->rewind();
var_dump($first->current()->getName());
var_dump(empty($first));
var_dump($first->current()->getName());

?>
--EXPECT--
--- var_dump ---
string(5) "first"
object(SimpleXMLElement)#2 (1) {
  [0]=>
  object(SimpleXMLElement)#4 (1) {
    ["foo"]=>
    object(SimpleXMLElement)#5 (0) {
    }
  }
}
string(5) "first"
--- empty ---
string(5) "first"
bool(false)
string(5) "first"

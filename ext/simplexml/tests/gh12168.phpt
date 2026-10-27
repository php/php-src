--TEST--
GH-12168 (Mixing processing instructions and element nodes gives inconsistent results in SimpleXML)
--EXTENSIONS--
simplexml
--FILE--
<?php

$sxe = simplexml_load_string('<container><x/><?hello world?></container>');
var_dump($sxe->children());

$sxe = simplexml_load_string('<container><?hello world?><x/></container>');
var_dump($sxe->children());

foreach ([
    '<c><!-- c --><x/></c>',
    '<c><x/><!-- c --></c>',
    '<c><?pi a?><x/><?pi b?><y/></c>',
    '<c>text</c>',
] as $xml) {
    $sxe = simplexml_load_string($xml);
    echo $xml, ': ', json_encode(array_keys((array) $sxe->children())), "\n";
}

?>
--EXPECTF--
object(SimpleXMLElement)#%d (2) {
  ["x"]=>
  object(SimpleXMLElement)#%d (0) {
  }
  ["hello"]=>
  object(SimpleXMLElement)#%d (0) {
  }
}
object(SimpleXMLElement)#%d (2) {
  ["hello"]=>
  object(SimpleXMLElement)#%d (0) {
  }
  ["x"]=>
  object(SimpleXMLElement)#%d (0) {
  }
}
<c><!-- c --><x/></c>: ["comment","x"]
<c><x/><!-- c --></c>: ["x","comment"]
<c><?pi a?><x/><?pi b?><y/></c>: ["pi","x","y"]
<c>text</c>: []

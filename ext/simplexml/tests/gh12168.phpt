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

/* Non-element nodes only show up alongside a matching element, consistent with (bool) */
foreach ([
    '<c><?pi a?></c>',
    '<c><!-- c --></c>',
    '<c>t<?pi a?></c>',
    '<!DOCTYPE c [<!ENTITY e "v">]><c>&e;<x/></c>',
] as $xml) {
    $sxe = simplexml_load_string($xml);
    echo $xml, ': ', json_encode(array_keys((array) $sxe->children())), ' ', var_export((bool) $sxe->children(), true), "\n";
}

$sxe = simplexml_load_string('<c xmlns:a="urn:a"><?pi a?><y/><a:x/></c>');
var_dump(array_keys((array) $sxe->children('urn:a')));
$sxe = simplexml_load_string('<c xmlns:a="urn:a"><?pi a?><y/></c>');
var_dump(array_keys((array) $sxe->children('urn:a')), (bool) $sxe->children('urn:a'));

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
<c><?pi a?></c>: [] false
<c><!-- c --></c>: [] false
<c>t<?pi a?></c>: [] false
<!DOCTYPE c [<!ENTITY e "v">]><c>&e;<x/></c>: ["e","x"] true
array(2) {
  [0]=>
  string(2) "pi"
  [1]=>
  string(1) "x"
}
array(0) {
}
bool(false)

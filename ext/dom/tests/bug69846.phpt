--TEST--
Bug #69846 Segmenation fault (access violation) when iterating over DOMNodeList
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$tmpDomDocument = new DOMDocument();
        
$xml = '<?xml version="1.0" encoding="UTF-8"?><dummy xmlns:xfa="http://www.xfa.org/schema/xfa-data/1.0/"><xfa:data>
  <form1>
    <TextField1>Value A</TextField1>
    <TextField1>Value B</TextField1>
    <TextField1>Value C</TextField1>
  </form1>
</xfa:data></dummy>';

$tmpDomDocument->loadXML($xml);

$dataNodes = $tmpDomDocument->firstChild->childNodes->item(0)->childNodes;

var_dump($dataNodes->length);
$datasetDom = new DOMDocument();

foreach ($dataNodes AS $node) {
    $node = $datasetDom->importNode($node, true);
    var_dump($node);
}

?>
===DONE===
--EXPECTF--
int(3)
object(DOMText)#%d (18) {
  ["wholeText"]=>
  string(3) "
  "
  ["data"]=>
  string(3) "
  "
  ["length"]=>
  int(3)
  ["nodeName"]=>
  string(5) "#text"
  ["nodeValue"]=>
  string(3) "
  "
  ["nodeType"]=>
  int(3)
  ["parentNode"]=>
  NULL
  ["childNodes"]=>
  NULL
  ["firstChild"]=>
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  NULL
  ["attributes"]=>
  NULL
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(3) "
  "
}
object(DOMElement)#%d (17) {
  ["tagName"]=>
  string(5) "form1"
  ["schemaTypeInfo"]=>
  NULL
  ["nodeName"]=>
  string(5) "form1"
  ["nodeValue"]=>
  string(39) "
    Value A
    Value B
    Value C
  "
  ["nodeType"]=>
  int(1)
  ["parentNode"]=>
  NULL
  ["childNodes"]=>
  string(22) "(object value omitted)"
  ["firstChild"]=>
  string(22) "(object value omitted)"
  ["lastChild"]=>
  string(22) "(object value omitted)"
  ["previousSibling"]=>
  NULL
  ["attributes"]=>
  string(22) "(object value omitted)"
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  string(5) "form1"
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(39) "
    Value A
    Value B
    Value C
  "
}
object(DOMText)#%d (18) {
  ["wholeText"]=>
  string(1) "
"
  ["data"]=>
  string(1) "
"
  ["length"]=>
  int(1)
  ["nodeName"]=>
  string(5) "#text"
  ["nodeValue"]=>
  string(1) "
"
  ["nodeType"]=>
  int(3)
  ["parentNode"]=>
  NULL
  ["childNodes"]=>
  NULL
  ["firstChild"]=>
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  NULL
  ["attributes"]=>
  NULL
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(1) "
"
}
===DONE===

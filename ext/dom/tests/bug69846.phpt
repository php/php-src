--TEST--
Bug #69846 Segmenation fault (access violation) when iterating over DOMNodeList
--EXTENSIONS--
dom
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
--EXPECTF--
int(3)
object(DOMText)#%d (21) {
  ["nodeValue"]=>
  string(3) "
  "
  ["prefix"]=>
  string(0) ""
  ["textContent"]=>
  string(3) "
  "
  ["wholeText"]=>
  string(3) "
  "
  ["data"]=>
  string(3) "
  "
  ["length"]=>
  int(3)
  ["previousElementSibling"]=>
  NULL
  ["nextElementSibling"]=>
  NULL
  ["nodeName"]=>
  string(5) "#text"
  ["nodeType"]=>
  int(3)
  ["parentNode"]=>
  NULL
  ["childNodes"]=>
  string(22) "(object value omitted)"
  ["firstChild"]=>
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  NULL
  ["nextSibling"]=>
  NULL
  ["attributes"]=>
  NULL
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
}
object(DOMElement)#%d (23) {
  ["nodeValue"]=>
  string(39) "
    Value A
    Value B
    Value C
  "
  ["prefix"]=>
  string(0) ""
  ["textContent"]=>
  string(39) "
    Value A
    Value B
    Value C
  "
  ["schemaTypeInfo"]=>
  NULL
  ["tagName"]=>
  string(5) "form1"
  ["firstElementChild"]=>
  string(22) "(object value omitted)"
  ["lastElementChild"]=>
  string(22) "(object value omitted)"
  ["childElementCount"]=>
  int(3)
  ["previousElementSibling"]=>
  NULL
  ["nextElementSibling"]=>
  NULL
  ["nodeName"]=>
  string(5) "form1"
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
  ["nextSibling"]=>
  NULL
  ["attributes"]=>
  string(22) "(object value omitted)"
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["localName"]=>
  string(5) "form1"
  ["baseURI"]=>
  NULL
}
object(DOMText)#%d (21) {
  ["nodeValue"]=>
  string(1) "
"
  ["prefix"]=>
  string(0) ""
  ["textContent"]=>
  string(1) "
"
  ["wholeText"]=>
  string(1) "
"
  ["data"]=>
  string(1) "
"
  ["length"]=>
  int(1)
  ["previousElementSibling"]=>
  NULL
  ["nextElementSibling"]=>
  NULL
  ["nodeName"]=>
  string(5) "#text"
  ["nodeType"]=>
  int(3)
  ["parentNode"]=>
  NULL
  ["childNodes"]=>
  string(22) "(object value omitted)"
  ["firstChild"]=>
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  NULL
  ["nextSibling"]=>
  NULL
  ["attributes"]=>
  NULL
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
}

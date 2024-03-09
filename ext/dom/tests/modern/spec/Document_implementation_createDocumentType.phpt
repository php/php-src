--TEST--
Document::$implementation createDocumentType
--EXTENSIONS--
dom
--FILE--
<?php

$test_matrix = [
    ['uri' => 'qname', 'public' => 'public', 'system' => 'system'],
    ['uri' => 'qname', 'public' => 'public', 'system' => ''],
    ['uri' => 'qname', 'public' => '', 'system' => 'system'],
    ['uri' => 'qname', 'public' => '', 'system' => ''],
];

foreach ($test_matrix as $test_item) {
    $dom = DOM\XMLDocument::createEmpty();
    $dtd = $dom->implementation->createDocumentType($test_item['uri'], $test_item['public'], $test_item['system']);

    var_dump($dtd);

    $dom->append($dom->importNode($dtd));

    echo $dom->saveXML(), "\n";
}

?>
--EXPECT--
object(DOM\DocumentType)#3 (23) {
  ["name"]=>
  string(5) "qname"
  ["entities"]=>
  string(22) "(object value omitted)"
  ["notations"]=>
  string(22) "(object value omitted)"
  ["publicId"]=>
  string(6) "public"
  ["systemId"]=>
  string(6) "system"
  ["internalSubset"]=>
  NULL
  ["nodeName"]=>
  string(5) "qname"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(10)
  ["parentNode"]=>
  NULL
  ["parentElement"]=>
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
  ["isConnected"]=>
  bool(false)
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE qname PUBLIC "public" "system">

object(DOM\DocumentType)#2 (23) {
  ["name"]=>
  string(5) "qname"
  ["entities"]=>
  string(22) "(object value omitted)"
  ["notations"]=>
  string(22) "(object value omitted)"
  ["publicId"]=>
  string(6) "public"
  ["systemId"]=>
  string(0) ""
  ["internalSubset"]=>
  NULL
  ["nodeName"]=>
  string(5) "qname"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(10)
  ["parentNode"]=>
  NULL
  ["parentElement"]=>
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
  ["isConnected"]=>
  bool(false)
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE qname PUBLIC "public" "">

object(DOM\DocumentType)#1 (23) {
  ["name"]=>
  string(5) "qname"
  ["entities"]=>
  string(22) "(object value omitted)"
  ["notations"]=>
  string(22) "(object value omitted)"
  ["publicId"]=>
  string(0) ""
  ["systemId"]=>
  string(6) "system"
  ["internalSubset"]=>
  NULL
  ["nodeName"]=>
  string(5) "qname"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(10)
  ["parentNode"]=>
  NULL
  ["parentElement"]=>
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
  ["isConnected"]=>
  bool(false)
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE qname SYSTEM "system">

object(DOM\DocumentType)#4 (23) {
  ["name"]=>
  string(5) "qname"
  ["entities"]=>
  string(22) "(object value omitted)"
  ["notations"]=>
  string(22) "(object value omitted)"
  ["publicId"]=>
  string(0) ""
  ["systemId"]=>
  string(0) ""
  ["internalSubset"]=>
  NULL
  ["nodeName"]=>
  string(5) "qname"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(10)
  ["parentNode"]=>
  NULL
  ["parentElement"]=>
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
  ["isConnected"]=>
  bool(false)
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE qname>

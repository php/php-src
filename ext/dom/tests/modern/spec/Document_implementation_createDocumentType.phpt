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
    $dom = Dom\XMLDocument::createEmpty();
    $dtd = $dom->implementation->createDocumentType($test_item['uri'], $test_item['public'], $test_item['system']);

    var_dump($dtd);

    $dom->append($dom->importNode($dtd));

    echo $dom->saveXml(), "\n";
}

?>
--EXPECT--
object(Dom\DocumentType)#3 (19) {
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
  ["nodeType"]=>
  int(10)
  ["nodeName"]=>
  string(5) "qname"
  ["baseURI"]=>
  NULL
  ["isConnected"]=>
  bool(false)
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
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE qname PUBLIC "public" "system">

object(Dom\DocumentType)#2 (19) {
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
  ["nodeType"]=>
  int(10)
  ["nodeName"]=>
  string(5) "qname"
  ["baseURI"]=>
  NULL
  ["isConnected"]=>
  bool(false)
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
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE qname PUBLIC "public" "">

object(Dom\DocumentType)#1 (19) {
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
  ["nodeType"]=>
  int(10)
  ["nodeName"]=>
  string(5) "qname"
  ["baseURI"]=>
  NULL
  ["isConnected"]=>
  bool(false)
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
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE qname SYSTEM "system">

object(Dom\DocumentType)#4 (19) {
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
  ["nodeType"]=>
  int(10)
  ["nodeName"]=>
  string(5) "qname"
  ["baseURI"]=>
  NULL
  ["isConnected"]=>
  bool(false)
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
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE qname>

--TEST--
Document::$implementation createDocument
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();

echo "--- (null, \"\") ---\n";

var_dump($dom->implementation->createDocument(null, ""));

echo "--- (null, \"qname\") ---\n";

echo $dom->implementation->createDocument(null, "qname")->saveXML(), "\n";

echo "--- (\"\", \"qname\") ---\n";

echo $dom->implementation->createDocument("", "qname")->saveXML(), "\n";

echo "--- (\"urn:a\", \"qname\") ---\n";

echo $dom->implementation->createDocument("urn:a", "qname")->saveXML(), "\n";

echo "--- With doctype ---\n";

$dtd = $dom->implementation->createDocumentType("mydoc", "", "");
echo $dom->implementation->createDocument(null, "", $dtd)->saveXML(), "\n";

echo "--- With auto-adopting doctype ---\n";

$dom2 = DOM\XMLDocument::createEmpty();
$dtd = $dom2->implementation->createDocumentType("mydoc", "", "");
echo $dom->implementation->createDocument(null, "", $dtd)->saveXML(), "\n";

?>
--EXPECT--
--- (null, "") ---
object(DOM\XMLDocument)#3 (30) {
  ["xmlEncoding"]=>
  string(5) "UTF-8"
  ["xmlStandalone"]=>
  bool(false)
  ["xmlVersion"]=>
  string(3) "1.0"
  ["formatOutput"]=>
  bool(false)
  ["implementation"]=>
  string(22) "(object value omitted)"
  ["URL"]=>
  string(11) "about:blank"
  ["documentURI"]=>
  string(11) "about:blank"
  ["characterSet"]=>
  string(5) "UTF-8"
  ["charset"]=>
  string(5) "UTF-8"
  ["inputEncoding"]=>
  string(5) "UTF-8"
  ["doctype"]=>
  NULL
  ["documentElement"]=>
  NULL
  ["firstElementChild"]=>
  NULL
  ["lastElementChild"]=>
  NULL
  ["childElementCount"]=>
  int(0)
  ["body"]=>
  NULL
  ["nodeType"]=>
  int(9)
  ["nodeName"]=>
  string(9) "#document"
  ["baseURI"]=>
  string(11) "about:blank"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  NULL
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
  NULL
}
--- (null, "qname") ---
<?xml version="1.0" encoding="UTF-8"?>
<qname/>
--- ("", "qname") ---
<?xml version="1.0" encoding="UTF-8"?>
<qname/>
--- ("urn:a", "qname") ---
<?xml version="1.0" encoding="UTF-8"?>
<qname xmlns="urn:a"/>
--- With doctype ---
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE mydoc>

--- With auto-adopting doctype ---
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE mydoc>

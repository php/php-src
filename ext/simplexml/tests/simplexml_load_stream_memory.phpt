--TEST--
simplexml_load_stream() - from memory stream
--EXTENSIONS--
simplexml
--FILE--
<?php

$tmp = fopen("php://memory", "w+");
fwrite($tmp, "<root><child1/><child2/></root>");
rewind($tmp);
$sxe1 = simplexml_load_stream($tmp);
rewind($tmp);
$sxe2 = simplexml_load_stream($tmp, document_uri: 'http://example.com');
fclose($tmp);

var_dump($sxe1, $sxe2);

?>
--EXPECTF--
object(SimpleXMLElement)#%d (2) {
  ["child1"]=>
  object(SimpleXMLElement)#%d (0) {
  }
  ["child2"]=>
  object(SimpleXMLElement)#%d (0) {
  }
}
object(SimpleXMLElement)#%d (2) {
  ["child1"]=>
  object(SimpleXMLElement)#%d (0) {
  }
  ["child2"]=>
  object(SimpleXMLElement)#%d (0) {
  }
}

--TEST--
XMLReader::fromString() - custom constructor
--EXTENSIONS--
xmlreader
--FILE--
<?php
class CustomXMLReader extends XMLReader {
    public int $myField;

    public function __construct() {
        $this->myField = 1234;
        echo "hello world\n";
    }
}

$reader = CustomXMLReader::fromString("<root/>");
try {
    var_dump($reader);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($reader->read());
var_dump($reader->nodeType);
?>
--EXPECTF--
hello world
object(CustomXMLReader)#%d (14) {
  ["myField"]=>
  int(1234)
  ["attributeCount"]=>
  int(0)
  ["baseURI"]=>
  string(0) ""
  ["depth"]=>
  int(0)
  ["hasAttributes"]=>
  bool(false)
  ["hasValue"]=>
  bool(false)
  ["isDefault"]=>
  bool(false)
  ["localName"]=>
  string(0) ""
  ["name"]=>
  string(0) ""
  ["namespaceURI"]=>
  string(0) ""
  ["nodeType"]=>
  int(0)
  ["prefix"]=>
  string(0) ""
  ["value"]=>
  string(0) ""
  ["xmlLang"]=>
  string(0) ""
}
Failed to read property because no XML data has been read yet
bool(true)
int(1)

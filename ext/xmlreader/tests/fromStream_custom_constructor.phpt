--TEST--
XMLReader::fromStream() - custom constructor
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

$h = fopen("php://memory", "w+");
fwrite($h, "<root/>");
fseek($h, 0);

$reader = CustomXMLReader::fromStream($h, encoding: "UTF-8");
var_dump($reader);
var_dump($reader->read());
var_dump($reader->nodeType);

fclose($h);
?>
--EXPECTF--
hello world
object(CustomXMLReader)#%d (1) {
  ["attributeCount"]=>
  uninitialized(int)
  ["baseURI"]=>
  uninitialized(string)
  ["depth"]=>
  uninitialized(int)
  ["hasAttributes"]=>
  uninitialized(bool)
  ["hasValue"]=>
  uninitialized(bool)
  ["isDefault"]=>
  uninitialized(bool)
  ["isEmptyElement"]=>
  uninitialized(bool)
  ["localName"]=>
  uninitialized(string)
  ["name"]=>
  uninitialized(string)
  ["namespaceURI"]=>
  uninitialized(string)
  ["nodeType"]=>
  uninitialized(int)
  ["prefix"]=>
  uninitialized(string)
  ["value"]=>
  uninitialized(string)
  ["xmlLang"]=>
  uninitialized(string)
  ["myField"]=>
  int(1234)
}
bool(true)
int(1)

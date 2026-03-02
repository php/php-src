--TEST--
XMLWriter::toMemory() - custom constructor
--EXTENSIONS--
xmlwriter
--FILE--
<?php

class CustomXMLWriter extends XMLWriter {
    public int $myField;

    public function __construct() {
        $this->myField = 1234;
        echo "hello world\n";
    }
}

$writer = CustomXMLWriter::toMemory();
var_dump($writer);
$writer->startElement("root");
$writer->endElement();
echo $writer->outputMemory();

?>
--EXPECTF--
hello world
object(CustomXMLWriter)#%d (1) {
  ["myField"]=>
  int(1234)
}
<root/>

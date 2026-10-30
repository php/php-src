--TEST--
XMLWriter::toStream() - custom constructor
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

$h = fopen("php://output", "w");

$writer = CustomXMLWriter::toStream($h);
var_dump($writer);
$writer->startElement("root");
$writer->endElement();
$writer->flush();

?>
--EXPECTF--
hello world
object(CustomXMLWriter)#%d (1) {
  ["myField"]=>
  int(1234)
}
<root/>

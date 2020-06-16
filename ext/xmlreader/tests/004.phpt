--TEST--
XMLReader: libxml2 XML Reader, attributes test
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php
$filename = __DIR__ . '/_004.xml';

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book num="1" idx="2">book1</book></books>';
file_put_contents($filename, $xmlstring);

$reader = new XMLReader();
if (!$reader->open($filename)) {
    exit();
}

while ($reader->read()) {
    if ($reader->nodeType != XMLREADER::END_ELEMENT) {
        echo $reader->name."\n";
        if ($reader->nodeType == XMLREADER::ELEMENT && $reader->hasAttributes) {
            $attr = $reader->moveToFirstAttribute();
            while ($attr) {
                echo "   Attribute Name: ".$reader->name."\n";
                echo "   Attribute Value: ".$reader->value."\n";
                $attr = $reader->moveToNextAttribute();
            }
        }
    }
}
$reader->close();
unlink($filename);
?>
--EXPECT--
books
book
   Attribute Name: num
   Attribute Value: 1
   Attribute Name: idx
   Attribute Value: 2
#text

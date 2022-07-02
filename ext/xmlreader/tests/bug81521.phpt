--TEST--
Bug #81521: XMLReader::getParserProperty may throw with a valid property
--FILE--
<?php
$reader = new XMLReader();
try {
    var_dump($reader->setParserProperty(XMLReader::LOADDTD, 1));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($reader->getParserProperty(XMLReader::LOADDTD));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot access parser properties before loading data
Cannot access parser properties before loading data

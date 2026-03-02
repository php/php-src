--TEST--
XMLReader::fromString() - custom constructor with error
--EXTENSIONS--
xmlreader
--FILE--
<?php
class CustomXMLReader extends XMLReader {
    public function __construct() {
        throw new Error('nope');
    }
}

try {
    CustomXMLReader::fromString("<root/>");
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
nope

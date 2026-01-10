--TEST--
XMLReader::fromStream() - custom constructor with error
--EXTENSIONS--
xmlreader
--FILE--
<?php
class CustomXMLReader extends XMLReader {
    public function __construct() {
        throw new Error('nope');
    }
}

$h = fopen("php://memory", "w+");
fwrite($h, "<root/>");
fseek($h, 0);

try {
    CustomXMLReader::fromStream($h, encoding: "UTF-8");
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

fclose($h);
?>
--EXPECT--
nope

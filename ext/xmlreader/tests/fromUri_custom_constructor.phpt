--TEST--
XMLReader::fromUri() - custom constructor
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

$filename = __DIR__ . '/_fromUri_custom_constructor.xml';
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books></books>';
file_put_contents($filename, $xmlstring);

$reader = XMLReader::fromUri($filename);

// Only go through
while ($reader->read()) {
    echo $reader->name."\n";
}
$reader->close();

?>
--EXPECT--
books
books
--CLEAN--
<?php
@unlink(__DIR__ . '/_fromUri_custom_constructor.xml');
?>

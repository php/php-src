--TEST--
XMLReader::fromUri() - custom constructor with error
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
    CustomXMLReader::fromUri("nonexistent");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$filename = __DIR__ . '/_fromUri_custom_constructor_error.xml';
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books></books>';
file_put_contents($filename, $xmlstring);

try {
    CustomXMLReader::fromUri($filename);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Unable to open source data
Error: nope
--CLEAN--
<?php
@unlink(__DIR__ . '/_fromUri_custom_constructor_error.xml');
?>

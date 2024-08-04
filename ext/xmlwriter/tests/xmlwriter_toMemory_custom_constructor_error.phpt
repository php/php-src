--TEST--
XMLWriter::toMemory() - custom constructor error
--EXTENSIONS--
xmlwriter
--FILE--
<?php

class CustomXMLWriter extends XMLWriter {
    public function __construct() {
        throw new Error('nope');
    }
}

try {
    CustomXMLWriter::toMemory();
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
nope

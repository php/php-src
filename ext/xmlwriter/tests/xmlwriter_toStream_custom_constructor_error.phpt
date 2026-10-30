--TEST--
XMLWriter::toStream() - custom constructor error
--EXTENSIONS--
xmlwriter
--FILE--
<?php

class CustomXMLWriter extends XMLWriter {
    public function __construct() {
        throw new Error('nope');
    }
}

$h = fopen("php://output", "w");

try {
    CustomXMLWriter::toStream($h);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
nope

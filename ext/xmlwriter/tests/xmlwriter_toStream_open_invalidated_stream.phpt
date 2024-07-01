--TEST--
XMLWriter::toStream() - open invalidated stream
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$h = fopen("php://output", "w");
fclose($h);

try {
    XMLWriter::toStream($h);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
XMLWriter::toStream(): supplied resource is not a valid stream resource

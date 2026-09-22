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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: XMLWriter::toStream(): supplied resource is not a valid stream resource

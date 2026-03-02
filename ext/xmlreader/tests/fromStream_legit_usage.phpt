--TEST--
XMLReader::fromStream() - legit usage
--EXTENSIONS--
xmlreader
--FILE--
<?php
$h = fopen("php://memory", "w+");
fwrite($h, "<root><!--my comment--><child/></root>");
fseek($h, 0);

$reader = XMLReader::fromStream($h, encoding: "UTF-8");
while ($reader->read()) {
    switch ($reader->nodeType) {
        case XMLReader::ELEMENT:
            echo "Element: ", $reader->name, "\n";
            break;
        case XMLReader::COMMENT:
            echo "Comment: ", $reader->value, "\n";
            break;
    }
}

// Force cleanup of stream reference
unset($reader);

var_dump(ftell($h));

fclose($h);
?>
--EXPECT--
Element: root
Comment: my comment
Element: child
int(38)

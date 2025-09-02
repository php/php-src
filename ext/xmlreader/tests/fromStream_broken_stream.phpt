--TEST--
XMLReader::fromStream() - broken stream
--EXTENSIONS--
xmlreader
--FILE--
<?php
$h = fopen("php://memory", "w+");
fwrite($h, "<root><!--my comment-->");
fseek($h, 0);

$reader = XMLReader::fromStream($h, encoding: "UTF-8");
$start = true;
while ($result = @$reader->read()) {
    var_dump($result);
    switch ($reader->nodeType) {
        case XMLReader::ELEMENT:
            echo "Element: ", $reader->name, "\n";
            break;
        case XMLReader::COMMENT:
            echo "Comment: ", $reader->value, "\n";
            break;
    }

    if ($start) {
        fwrite($h, "<child/></root>");
        fclose($h);
        $start = false;
    }
}
var_dump($reader->depth);
?>
--EXPECT--
bool(true)
Element: root
bool(true)
Comment: my comment
int(1)

--TEST--
GH-14183 (XMLReader::open() can't be overridden)
--EXTENSIONS--
xmlreader
--FILE--
<?php
class MyXMLReader extends XMLReader
{
    public static function open(string $uri, string $encoding = null, int $flags = 0): bool|\XMLReader
    {
        echo 'overridden', PHP_EOL;
        return true;
    }
}

var_dump(MyXMLReader::open('asdf'));
$o = new MyXMLReader;
var_dump($o->open('asdf'));
?>
--EXPECT--
overridden
bool(true)
overridden
bool(true)

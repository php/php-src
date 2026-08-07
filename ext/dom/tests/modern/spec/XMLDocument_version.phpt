--TEST--
XMLDocument::$version
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root><child/></root>');
var_dump($dom->xmlVersion);

foreach (['0.1', '1.0', '1.1', '', 'foo'] as $version) {
    try {
        $dom->xmlVersion = $version;
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($dom->xmlVersion);
}

?>
--EXPECT--
string(3) "1.0"
ValueError: Invalid XML version
string(3) "1.0"
string(3) "1.0"
string(3) "1.1"
ValueError: Invalid XML version
string(3) "1.1"
ValueError: Invalid XML version
string(3) "1.1"

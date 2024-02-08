--TEST--
XMLDocument::$version
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root><child/></root>');
var_dump($dom->xmlVersion);

foreach (['0.1', '1.0', '1.1', '', 'foo'] as $version) {
    try {
        $dom->xmlVersion = $version;
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($dom->xmlVersion);
}

?>
--EXPECT--
string(3) "1.0"
Invalid XML version
string(3) "1.0"
string(3) "1.0"
string(3) "1.1"
Invalid XML version
string(3) "1.1"
Invalid XML version
string(3) "1.1"

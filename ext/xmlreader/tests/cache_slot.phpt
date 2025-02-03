--TEST--
Cache slot test
--EXTENSIONS--
xmlreader
--FILE--
<?php

class Test1 {
    function __construct(public string $localName) {}
}

#[AllowDynamicProperties]
class Test2 extends XMLReader {
}

function readLocalName($obj) {
    for ($i = 0; $i < 2; $i++)
        var_dump($obj->localName);
}

function readTestProp($obj) {
    for ($i = 0; $i < 2; $i++)
        var_dump($obj->testProp);
}

$reader = XMLReader::fromString("<root/>");
$reader->read();
$test1 = new Test1("hello");

readLocalName($reader);
readLocalName($test1);
readLocalName($reader);

$test2 = new Test2;
$test2->testProp = 1;

readTestProp($test2);
readTestProp($reader);
readTestProp($test2);

?>
--EXPECTF--
string(4) "root"
string(4) "root"
string(5) "hello"
string(5) "hello"
string(4) "root"
string(4) "root"
int(1)
int(1)

Warning: Undefined property: XMLReader::$testProp in %s on line %d
NULL

Warning: Undefined property: XMLReader::$testProp in %s on line %d
NULL
int(1)
int(1)

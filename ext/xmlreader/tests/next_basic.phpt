--TEST--
XMLReader: next basic
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip XMLReader extension required"; ?>
--FILE--
<?php

$xml = '<?xml version="1.0" encoding="UTF-8"?><nodes><node1><sub /></node1>
<node2><sub /></node2><node3><sub /></node3><node4><sub /></node4></nodes>';

$reader = new XMLReader();
$reader->read();
$reader->next();
$reader->close();

$reader->XML($xml);
$reader->read();
$reader->read();
echo $reader->name . PHP_EOL;
var_dump($reader->next('node3'));
echo $reader->name . PHP_EOL;
var_dump($reader->next());
echo $reader->name . PHP_EOL;
var_dump($reader->next('node5'));
echo $reader->name . PHP_EOL;
var_dump($reader->next());
echo $reader->name . PHP_EOL;
$reader->close();

?>
--EXPECTF--
Warning: XMLReader::read(): Load Data before trying to read in %s on line %d

Warning: XMLReader::next(): Load Data before trying to read in %s on line %d
node1
bool(true)
node3
bool(true)
node4
bool(false)

bool(false)

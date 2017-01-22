--TEST--
Bug #54138 - DOMNode::getLineNo() doesn't return line number higher than 65535
--SKIPIF--
<?php
if (!extension_loaded('dom')) die('skip dom extension not available');
if (!defined('LIBXML_BIGLINES')) die('skip this test requires LIBXML_BIGLINES');
?>
--FILE--
<?php
$foos = str_repeat('<foo/>' . PHP_EOL, 65535);
$xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<root>
$foos
<bar/>
</root>
XML;
$dom = new DOMDocument();
$dom->loadXML($xml, LIBXML_BIGLINES);
var_dump($dom->getElementsByTagName('bar')->item(0)->getLineNo());
?>
--EXPECT--
int(65540)

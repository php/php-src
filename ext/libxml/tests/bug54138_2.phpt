--TEST--
Bug #54138 - DOMNode::getLineNo() doesn't return line number higher than 65535
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (LIBXML_VERSION < 20900) die('skip this test is for libxml >= 2.9.0 only');
if (defined('LIBXML_BIGLINES')) die('skip this test is for LIBXML_BIGLINES being undefined');
?>
--FILE--
<?php
define('LIBXML_BIGLINES', 1<<22);
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

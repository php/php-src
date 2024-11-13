--TEST--
DOMXPath::evaluate() with PHP function passing a namespace node-set
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <p>hi</p>
</container>
XML);

$xpath = new DOMXPath($dom);

function node_test($nodes) {
    echo "nodes count: ", count($nodes), "\n";
    return array_sum(array_map(fn ($node) => strlen($node->nodeName), $nodes));
}

$xpath->registerNamespace("php", "http://php.net/xpath");
$xpath->registerPhpFunctions(['node_test']);
var_dump($xpath->evaluate('number(php:function("node_test", //namespace::*))'));
var_dump($xpath->evaluate('boolean(php:function("node_test", //namespace::*))'));

?>
--EXPECT--
nodes count: 2
float(18)
nodes count: 2
bool(true)

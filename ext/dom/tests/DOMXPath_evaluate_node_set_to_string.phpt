--TEST--
DOMXPath::evaluate() with PHP function passing node-set returning a string
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

$xpath->registerNamespace("php", "http://php.net/xpath");
$xpath->registerPhpFunctions(['strrev']);
var_dump($xpath->evaluate('php:functionString("strrev", //p)'));
var_dump($xpath->evaluate('php:functionString("strrev", //namespace::*)'));

?>
--EXPECT--
string(2) "ih"
string(36) "ecapseman/8991/LMX/gro.3w.www//:ptth"

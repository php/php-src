--TEST--
DOMImplementation::hasFeature()
--EXTENSIONS--
dom
--FILE--
<?php
$imp = new DOMImplementation();
var_dump($imp->hasFeature('Core', '1.0'));
var_dump($imp->hasFeature('XML', '2.0'));
?>
--EXPECT--
bool(true)
bool(true)

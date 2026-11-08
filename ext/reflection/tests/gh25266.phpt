--TEST--
GH-25266 (Misaligned zval read in ReflectionProperty::isReadable() on internal virtual property)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
dom
--FILE--
<?php

$rp = new ReflectionProperty(DOMDocument::class, 'nodeName');
var_dump($rp->isReadable(null, new DOMDocument));

?>
--EXPECT--
bool(true)

--TEST--
GH-21688 (SEGV in C14N on empty HTMLDocument)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();
var_dump($dom->C14N());
?>
--EXPECT--
string(0) ""

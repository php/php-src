--TEST--
DOM\HTML5Document::loadHTML() - empty document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();

$dom->loadHTML('');
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<html><head></head><body></body></html>

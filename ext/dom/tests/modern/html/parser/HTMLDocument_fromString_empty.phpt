--TEST--
DOM\HTMLDocument::createFromString() - empty document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString('');
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<html><head></head><body></body></html>

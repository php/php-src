--TEST--
Dom\HTMLDocument::createFromString() - empty document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString('');
echo $dom->saveHtml(), "\n";

?>
--EXPECT--
<html><head></head><body></body></html>

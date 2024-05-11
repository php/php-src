--TEST--
Dom\HTMLDocument::createFromString() - document without body
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString('<!doctype html><p align="\'">foo &apos;</p>');
echo $dom->saveHtml(), "\n";

?>
--EXPECT--
<!DOCTYPE html><html><head></head><body><p align="'">foo '</p></body></html>

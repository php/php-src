--TEST--
DOM\HTMLDocument::createFromString() - document without body
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString('<!doctype html><p align="\'">foo &apos;</p>');
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<!DOCTYPE html><html><head></head><body><p align="'">foo '</p></body></html>

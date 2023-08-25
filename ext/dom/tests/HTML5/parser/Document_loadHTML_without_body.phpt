--TEST--
DOM\HTML5Document::loadHTML() - document without body
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();

$html = '<!doctype html><p align="\'">foo &apos;</p>';
$dom->loadHTML($html);
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<!DOCTYPE html><html><head></head><body><p align="'">foo '</p></body></html>

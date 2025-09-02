--TEST--
template content indirect removal
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (!PHP_DEBUG) { die ("skip only for debug build"); }
?>
--FILE--
<?php
$dom = Dom\HTMLDocument::createFromString('<template>foo<template>nested</template></template>', LIBXML_NOERROR);
$head = $dom->head;
var_dump($dom->debugGetTemplateCount());
$head->remove();
var_dump($dom->debugGetTemplateCount());
unset($head);
var_dump($dom->debugGetTemplateCount());
?>
--EXPECT--
int(2)
int(2)
int(0)

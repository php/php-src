--TEST--
Bug #77040 (tidyNode::isHtml() is completely broken)
--SKIPIF--
<?php
if (!extension_loaded('tidy')) die('skip tidy extension not available');
?>
--FILE--
<?php
$tidy = new tidy;
$tidy->parseString("<p>text</p>");
$p = $tidy->body()->child[0]->child[0];
var_dump($p->type === TIDY_NODETYPE_TEXT);
var_dump($p->isHtml());
?>
--EXPECT--
bool(true)
bool(false)

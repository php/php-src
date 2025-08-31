--TEST--
Bug #77040 (tidyNode::isHtml() is completely broken)
--EXTENSIONS--
tidy
--FILE--
<?php
$tidy = new tidy;
$tidy->parseString("<p>text</p><p><![CDATA[cdata]]></p>");
$p = $tidy->body()->child[0];
var_dump($p->type === TIDY_NODETYPE_START);
var_dump($p->isHtml());
$text = $p->child[0];
var_dump($text->type === TIDY_NODETYPE_TEXT);
var_dump($text->isHtml());
$cdata = $tidy->body()->child[1]->child[0];
var_dump($cdata->type === TIDY_NODETYPE_CDATA);
var_dump($cdata->isHtml());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
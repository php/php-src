--TEST--
Bug #78221 (DOMNode::normalize() doesn't remove empty text nodes)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadHTML('<p id=x>foo</p>');
$p = $doc->getElementById('x');
$p->childNodes[0]->textContent = '';
$p->normalize();
var_dump($p->childNodes->length);
?>
--EXPECT--
int(0)

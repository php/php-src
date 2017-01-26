--TEST--
Bug #66502 (DOM document dangling reference)
--SKIPIF--
<?php
if (!extension_loaded('dom')) die('skip requires ext/dom');
?>
--FILE--
<?php
$dom = new DOMDocument('1.0', 'UTF-8');
$element = $dom->appendChild(new DOMElement('root'));
$comment = new DOMComment("Comment 0");
$comment = $element->appendChild($comment);

$comment->__construct("Comment 1");
$comment->__construct("Comment 2");
$comment->__construct("Comment 3");
echo 'DONE', PHP_EOL;
?>
--EXPECT--
DONE
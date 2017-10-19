--TEST--
Ensure that DOMNode::compareDocumentPosition triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$node = new DOMNode('string');
$node->compareDocumentPosition();
?>
--EXPECTF--	
Warning: DOMNode::compareDocumentPosition(): Not yet implemented in %s on line %d
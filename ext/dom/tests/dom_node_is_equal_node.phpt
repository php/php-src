--TEST--
Ensure that DOMNode::isEqualNode triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$node = new DOMNode('div');
$foo = $node->isEqualNode(new DOMNode());
?>
--EXPECTF--	
Warning: DOMNode::isEqualNode(): Not yet implemented in %s on line %d
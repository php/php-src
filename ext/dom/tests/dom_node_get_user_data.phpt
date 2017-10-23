--TEST--
Ensure that DOMNode::getUserData triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$node = new DOMNode('div');
$node->getUserData();
?>
--EXPECTF--	
Warning: DOMNode::getUserData(): Not yet implemented in %s on line %d
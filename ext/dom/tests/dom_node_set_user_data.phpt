--TEST--
Ensure that DOMNode::setUserData triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$node = new DOMNode('div');
$node->setUserData('test');
?>
--EXPECTF--	
Warning: DOMNode::setUserData(): Not yet implemented in %s on line %d
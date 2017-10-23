--TEST--
Ensure that DOMNode::getFeature triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$node = new DOMNode('div');
$feature = $node->getFeature();
?>
--EXPECTF--	
Warning: DOMNode::getFeature(): Not yet implemented in %s on line %d
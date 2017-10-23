--TEST--
Ensure that DOMText::replaceWholeText triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$element = new DOMText('string');
$element->replaceWholeText('replacement');
?>
--EXPECTF--	
Warning: DOMText::replaceWholeText(): Not yet implemented in %s on line %d
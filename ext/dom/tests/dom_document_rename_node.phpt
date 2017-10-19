--TEST--
Ensure that DOMDocument::renameNode triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$xml = new DOMDocument( "1.0", "ISO-8859-15" );
$xml->renameNode(new DOMNode());
?>
--EXPECTF--	
Warning: DOMDocument::renameNode(): Not yet implemented in %s on line %d
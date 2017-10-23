--TEST--
Ensure that DOMDocument::adoptNode triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFEst 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$xml = new DOMDocument( "1.0", "ISO-8859-15" );
$xml->adoptNode(new DOMNode());
?>
--EXPECTF--	
Warning: DOMDocument::adoptNode(): Not yet implemented in %s on line %d
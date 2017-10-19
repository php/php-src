--TEST--
Ensure that DOMNameList::getName triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$nm = new DOMNameList();
echo $nm->getName();
?>
--EXPECTF--	
Warning: DOMNameList::getName(): Not yet implemented in %s on line %d
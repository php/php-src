--TEST--
cloning uncloneable object
--SKIPIF--
<?php if (!extension_loaded("xsl")) die("skip xsl extension is missing");?>
--INI--
zend.ze1_compatibility_mode=1
--FILE--
<?php

$new = &new XSLTProcessor(); 
var_dump($new);

echo "Done\n";
?>
--EXPECTF--	
Strict Standards: Assigning the return value of new by reference is deprecated in %s on line %d

Strict Standards: Implicit cloning object of class 'XSLTProcessor' because of 'zend.ze1_compatibility_mode' in %s on line %d

Fatal error: Trying to clone uncloneable object of class XSLTProcessor in Unknown on line 0

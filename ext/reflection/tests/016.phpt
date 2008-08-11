--TEST--
ReflectionExtension::getDependencies()
--SKIPIF--
<?php
extension_loaded('reflection') or die('skip'); 
if (!extension_loaded("xml")) {
  die('skip xml extension not available');
}
?>
--FILE--
<?php
$ext = new ReflectionExtension("xml");
$deps = $ext->getDependencies();
var_dump($deps);
?>
--EXPECT--	
array(1) {
  ["libxml"]=>
  string(8) "Required"
}

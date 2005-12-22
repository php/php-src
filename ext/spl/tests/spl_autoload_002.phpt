--TEST--
SPL: spl_autoloadfunctions()
--SKIPIF--
<?php 
	if (!extension_loaded("spl")) die ("skip");
	if (spl_autoload_functions() !== false) die('skip __autoload() registered by php.ini'); 
?>
--FILE--
<?php

function SplAutoloadTest1($name) {}
function SplAutoloadTest2($name) {}

var_dump(spl_autoload_functions());

spl_autoload_register();

var_dump(spl_autoload_functions());

spl_autoload_register('SplAutoloadTest1');
spl_autoload_register('SplAutoloadTest2');
spl_autoload_register('SplAutoloadTest1');

var_dump(spl_autoload_functions());

spl_autoload_unregister('SplAutoloadTest1');

var_dump(spl_autoload_functions());

spl_autoload_unregister('spl_autoload_call');

var_dump(spl_autoload_functions());

spl_autoload_register();

var_dump(spl_autoload_functions());

spl_autoload_unregister('spl_autoload');

var_dump(spl_autoload_functions());

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
bool(false)
array(1) {
  [0]=>
  string(12) "spl_autoload"
}
array(3) {
  [0]=>
  string(12) "spl_autoload"
  [1]=>
  string(16) "SplAutoloadTest1"
  [2]=>
  string(16) "SplAutoloadTest2"
}
array(2) {
  [0]=>
  string(12) "spl_autoload"
  [1]=>
  string(16) "SplAutoloadTest2"
}
bool(false)
array(1) {
  [0]=>
  string(12) "spl_autoload"
}
bool(false)
===DONE===

--TEST--
Bug #72402: _php_mb_regex_ereg_replace_exec - double free
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
function throwit() {
	throw new Exception('it');
}
$var10 = "throwit";
try {
	$var14 = mb_ereg_replace_callback("", $var10, "");
} catch(Exception $e) {}
?>
DONE
--EXPECT--
DONE
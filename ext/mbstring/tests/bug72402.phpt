--TEST--
Bug #72402: _php_mb_regex_ereg_replace_exec - double free
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_ereg_replace_callback')) die('skip mb_ereg_replace_callback() not available');
?>
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

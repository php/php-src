--TEST--
Test trait_exists() function : usage variations  - case sensitivity
--FILE--
<?php
/* Prototype  : proto bool trait_exists(string traitname [, bool autoload])
 * Description: Checks if the trait exists
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

trait caseSensitivityTest {}
var_dump(trait_exists('casesensitivitytest'));

echo "Done"
?>
--EXPECTF--
bool(true)
Done

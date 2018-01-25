--TEST--
Test interface_exists() function : test autoload default value
--FILE--
<?php
/* Prototype  : bool interface_exists(string classname [, bool autoload])
 * Description: Checks if the class exists
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "*** Testing interface_exists() : test autoload default value ***\n";

spl_autoload_register(function ($class_name) {
    require_once $class_name . '.inc';
});

var_dump(interface_exists("AutoInterface"));

echo "\nDONE\n";

?>
--EXPECTF--
*** Testing interface_exists() : test autoload default value ***
bool(true)

DONE

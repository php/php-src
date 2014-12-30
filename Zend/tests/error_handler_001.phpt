--TEST--
error handler tests - 1
--FILE--
<?php

var_dump(get_error_handler());

var_dump(set_error_handler("my_error_handler"));

trigger_error("test", E_USER_WARNING);

var_dump(restore_error_handler());

var_dump(get_error_handler());

function my_error_handler($errno, $errstr, $errfile, $errline) {
        var_dump($errno, $errstr, $errfile, $errline);
}

echo "Done\n";
?>
--EXPECTF--
NULL
NULL
int(512)
string(4) "test"
string(%d) "%s/Zend/tests/error_handler_001.php"
int(7)
bool(true)
NULL
Done


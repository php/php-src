--TEST--
Bug #47320 ($php_errormsg out of scope in functions)
--INI--
display_errors=0
track_errors=1
--FILE--
<?php
if (!@substr('no 2nd parameter')) {
    echo '$php_errormsg in global: ' . $php_errormsg . "\n";
}

function foo() {
    if (!@strpos('no 2nd parameter')) {
        echo '$php_errormsg in function: ' . $php_errormsg . "\n";

        echo '$GLOBALS[php_errormsg] in function: ' .
                $GLOBALS['php_errormsg'] . "\n";
    }
}

foo();
?>
--EXPECT--
$php_errormsg in global: substr() expects at least 2 parameters, 1 given
$php_errormsg in function: strpos() expects at least 2 parameters, 1 given
$GLOBALS[php_errormsg] in function: substr() expects at least 2 parameters, 1 given

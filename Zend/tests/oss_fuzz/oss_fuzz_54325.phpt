--TEST--
oss-fuzz #54325: Fix use-after-free of name in var-var with malicious error handler
--FILE--
<?php
set_error_handler(function ($errno, $errstr) {
    var_dump($errstr);
    global $x;
    $x = new stdClass;
});

// Needs to be non-interned string
$x = strrev('foo');
$$x++;
var_dump($x);
?>
--EXPECT--
string(23) "Undefined variable $oof"
object(stdClass)#2 (0) {
}

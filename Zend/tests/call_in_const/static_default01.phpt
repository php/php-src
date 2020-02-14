--TEST--
Can call internal functions from defaults of static variables
--FILE--
<?php
function main() {
    static $call = SPRINTF("%s!", sprintf("Hello, %s", "World"));
    echo "$call\n";
}
main();
main();
// Note: It may be possible to allowing literally any call or value in static variables, but this is outside of the scope of this RFC.
?>
--EXPECTF--
Fatal error: Constant expression uses function SPRINTF() which is not in get_const_expr_functions() in %s on line 3

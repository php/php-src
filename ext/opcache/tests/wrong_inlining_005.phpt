--TEST--
Inlining of functions with ref arguments
--FILE--
<?php

function by_ref(&$var)
{
}
function &get_array() {
    $array = [new stdClass];
    return $array;
}
function test()
{
    by_ref(get_array()[0]);
    print "ok!\n";
}
test();

?>
--EXPECT--
ok!

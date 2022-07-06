--TEST--
exit() may occur in a call argument
--FILE--
<?php

function test($c) {
    if ($c) {
        var_dump(var_dump(exit("exit\n")));
    } else {
        var_dump("test");
    }
}
test(false);
test(true);

?>
--EXPECT--
string(4) "test"
exit

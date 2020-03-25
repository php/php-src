--TEST--
Bug #77191: Assertion failure in dce_live_ranges() when silencing is used
--FILE--
<?php
function test($x) {
    switch (@$x['y']) {
        case 1: return 'a';
        case 2: return 'b';
        case 3: return 'c';
        case 4: return 'd';
    }
    return 'e';
}
var_dump(test([]));
?>
--EXPECT--
string(1) "e"

--TEST--
zend_optimizer_replace_by_const(): OP2 uses of a temporary kept alive by FETCH_LIST_R
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
--FILE--
<?php
function test($v) {
    switch ($v) {
        case [$x] = (int)1.5:
            return "matched " . var_export($x, true);
        default:
            return "default";
    }
}
var_dump(test(1));
var_dump(test(2));
?>
--EXPECT--
string(12) "matched NULL"
string(7) "default"

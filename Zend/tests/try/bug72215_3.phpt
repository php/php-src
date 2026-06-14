--TEST--
Bug #72215.3 (Wrong return value if var modified in finally)
--FILE--
<?php
function &test() {
    try {
        return $a;
    } finally {
        $a = 2;
    }
}
var_dump(test());
?>
--EXPECT--
int(2)

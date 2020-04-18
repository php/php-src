--TEST--
Check that SCCP correctly handles non-terminating frees of loop variables
--FILE--
<?php
function test() {
    $arr = [];
    foreach ($arr as $item) {
        if (!empty($result)) {
            return $result;
        }
    }
    return 2;
}

var_dump(test());
?>
--EXPECT--
int(2)

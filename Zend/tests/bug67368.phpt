--TEST--
Bug #67368 (Memory leak with immediately dereferenced array in class constant)
--FILE--
<?php
class FooBar {
        const bar = ["bar" => 3]["bar"];
}
echo "okey";
?>
--EXPECT--
okey

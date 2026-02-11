--TEST--
scandir() flag validation and behavior
--FILE--
<?php

echo "== Valid flags ==\n";

$valid_flags = [
    SCANDIR_SORT_ASCENDING,
    SCANDIR_SORT_NONE,
];

foreach ($valid_flags as $flag) {
    $result = scandir(__DIR__, $flag);
    echo is_array($result) ? "OK\n" : "FAIL\n";
}

echo "== Invalid flags ==\n";

$invalid_flags = [-1, 3, 999, PHP_INT_MAX];

foreach ($invalid_flags as $flag) {
    try {
        scandir(__DIR__, $flag);
    } catch (ValueError $e) {
        echo $e->getMessage() . PHP_EOL;
    }
}

?>
--EXPECT--
== Valid flags ==
OK
OK
== Invalid flags ==
scandir(): Argument #2 ($sorting_order) must be one of PHP_SCANDIR_SORT_ASCENDING, or SCANDIR_SORT_NONE
scandir(): Argument #2 ($sorting_order) must be one of PHP_SCANDIR_SORT_ASCENDING, or SCANDIR_SORT_NONE
scandir(): Argument #2 ($sorting_order) must be one of PHP_SCANDIR_SORT_ASCENDING, or SCANDIR_SORT_NONE
scandir(): Argument #2 ($sorting_order) must be one of PHP_SCANDIR_SORT_ASCENDING, or SCANDIR_SORT_NONE
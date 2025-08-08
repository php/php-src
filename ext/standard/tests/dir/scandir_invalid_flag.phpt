--TEST--
Provide wrong flags to scandir()
--FILE--
<?php
try {
    scandir('something', -1);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
scandir(): Argument #2 ($sorting_order) must be one of the SCANDIR_SORT_ASCENDING, SCANDIR_SORT_DESCENDING and SCANDIR_SORT_NONE constants

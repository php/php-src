--TEST--
Calling closedir() without argument and without opening a directory beforehand
--FILE--
<?php
try {
    closedir();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Deprecated: closedir(): Passing null is deprecated, instead the last opened directory stream should be provided in %s on line %d
TypeError: No resource supplied

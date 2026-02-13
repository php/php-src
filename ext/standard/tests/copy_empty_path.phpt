--TEST--
copy() throws ValueError when source or destination is empty
--FILE--
<?php

$dir = __DIR__;

file_put_contents($dir . "/foo.txt", "test");

try {
    copy("", $dir . "/bar.txt");
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    copy($dir . "/foo.txt", "");
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--CLEAN--
<?php
$dir = __DIR__;
@unlink($dir . "/foo.txt");
@unlink($dir . "/bar.txt");
?>
--EXPECT--
Path must not be empty
copy(): Argument #2 ($to) cannot be empty

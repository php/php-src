--TEST--
Removing a block that has duplicate successors
--EXTENSIONS--
opcache
--FILE--
<?php
function test($foo) {
    $bar = 0;
    if ($bar === 1 && $foo && PHP_SAPI !== 'cli') {
        echo "foo\n";
    }
    echo "bar\n";
}
test(1);
?>
--EXPECT--
bar

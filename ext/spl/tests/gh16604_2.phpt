--TEST--
GH-16604 (Memory leaks in SPL constructors) - SplFileObject
--FILE--
<?php

file_put_contents(__DIR__.'/gh16604_2.tmp', 'hello');

$obj = new SplFileObject(__DIR__.'/gh16604_2.tmp');
try {
    $obj->__construct(__DIR__.'/gh16604_2.tmp');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--CLEAN--
<?php
@unlink(__DIR__.'/gh16604_2.tmp');
?>
--EXPECT--
Cannot call constructor twice

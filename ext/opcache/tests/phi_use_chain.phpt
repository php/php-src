--TEST--
Check that phi use chains are correctly maintained when removing blocks
--EXTENSIONS--
opcache
--FILE--
<?php

function test(array $adapters) {
    foreach ($adapters as $adapter) {
        if (\in_array('cli-server', ['cli', 'phpdbg'], true) && $adapter instanceof stdClass && !\filter_var('1', \FILTER_VALIDATE_BOOLEAN)) {
            continue;
        }

        $adapters[] = $adapter;
    }
}

?>
===DONE===
--EXPECT--
===DONE===

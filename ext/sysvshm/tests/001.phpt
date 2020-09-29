--TEST--
ftok() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

try {
    ftok("","");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    ftok(-1, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    ftok("qwertyu","qwertyu");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(ftok("nonexistentfile","q"));

var_dump(ftok(__FILE__,"q"));

echo "Done\n";
?>
--EXPECTF--
ftok(): Argument #1 ($filename) cannot be empty
ftok(): Argument #2 ($project_id) must be a single character
ftok(): Argument #2 ($project_id) must be a single character

Warning: ftok(): ftok() failed - No such file or directory in %s on line %d
int(-1)
int(%d)
Done

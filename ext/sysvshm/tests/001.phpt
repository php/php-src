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
    var_dump(ftok("",""));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(ftok(-1, -1));
var_dump(ftok("qwertyu","qwertyu"));

var_dump(ftok("nonexistentfile","q"));

var_dump(ftok(__FILE__,"q"));

echo "Done\n";
?>
--EXPECTF--
ftok(): Argument #1 ($pathname) cannot be empty

Warning: ftok(): Project identifier is invalid in %s on line %d
int(-1)

Warning: ftok(): Project identifier is invalid in %s on line %d
int(-1)

Warning: ftok(): ftok() failed - No such file or directory in %s on line %d
int(-1)
int(%d)
Done

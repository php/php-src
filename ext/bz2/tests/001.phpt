--TEST--
bzopen() and invalid parameters
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php

try {
    var_dump(bzopen("", "r"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(bzopen("", "w"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(bzopen("no_such_file", ""));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(bzopen("no_such_file", "x"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(bzopen("no_such_file", "rw"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(bzopen("no_such_file", "r"));

$fp = fopen(__FILE__,"r");
var_dump(bzopen($fp, "r"));

?>
--EXPECTF--
bzopen(): Argument #1 ($file) cannot be empty
bzopen(): Argument #1 ($file) cannot be empty
bzopen(): Argument #2 ($mode) must be either "r" or "w"
bzopen(): Argument #2 ($mode) must be either "r" or "w"
bzopen(): Argument #2 ($mode) must be either "r" or "w"

Warning: bzopen(no_such_file): Failed to open stream: No such file or directory in %s on line %d
bool(false)
resource(%d) of type (stream)

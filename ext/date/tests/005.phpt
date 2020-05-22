--TEST--
idate() and invalid params
--FILE--
<?php
date_default_timezone_set('UTC');

$t = mktime(0,0,0, 6, 27, 2006);

try {
    var_dump(idate(1,1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(idate(""));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(idate(0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(idate("B", $t));

try {
    var_dump(idate("[", $t));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(idate("'"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
idate(): Argument #1 ($format) must be a valid date format token
idate(): Argument #1 ($format) must be one character
idate(): Argument #1 ($format) must be a valid date format token
int(41)
idate(): Argument #1 ($format) must be a valid date format token
idate(): Argument #1 ($format) must be a valid date format token
Done

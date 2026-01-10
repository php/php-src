--TEST--
Test mb_ereg() function : mb_ereg 'successfully' matching incorrectly
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
/*
 * mb_ereg 'successfully' matching incorrectly:
 * Bug now seems to be fixed - error message is now generated when an 'empty'
 * pattern is supplied to mb_ereg. Similar error message to ereg().
 */

$input = '';
echo "Without \$regs arg:\n";
try {
    var_dump( mb_ereg($input, 'hello, world') );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "With \$regs arg:\n";
try {
    var_dump(mb_ereg($input, 'hello, world', $mb_regs));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump($mb_regs);
?>
--EXPECT--
Without $regs arg:
mb_ereg(): Argument #1 ($pattern) must not be empty
With $regs arg:
mb_ereg(): Argument #1 ($pattern) must not be empty
NULL

--TEST--
phpdbg_watch()
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) {
    die("xfail There may be flaws in the implementation of watchpoints that cause failures");
}
if (getenv('SKIP_ASAN')) {
    die("skip intentionally causes segfaults");
}
?>
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> $foo: 1
$foo: 2
$foo: foo
$foo: for
$foo REMOVED
$foo changed
[Script ended normally]
prompt>
--FILE--
<?php

class A {
    public $foo;
}

$a = new A;
$handle = phpdbg_watch('$foo', $a, function($name, $type, $value = null) {
    if ($type == PhpdbgWatchModification::UPDATED) {
        print "$name: $value\n";
    } else {
        print "$name REMOVED\n";
    }
});

$a->foo = 1;
++$a->foo;

$a->foo = "foo";
$a->foo[2] = "r";

unset($a);

$a = new A;

$handle = phpdbg_watch('$foo', $a, function($name) {
    print "$name changed";
});

$a->foo = "foo";

phpdbg_unwatch($handle);

$a->foo[2] = "x";

?>
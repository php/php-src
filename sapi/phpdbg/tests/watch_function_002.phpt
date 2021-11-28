--TEST--
phpdbg_watch_recursive()
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
prompt> UPDATED: $->foo: array ( 0 => 1, )
ADDED: 1: array ( 0 => 1, 1 => 10, )
ADDED: 0: array ( 0 => 1, 1 => 10, )
UPDATED: $->foo[]: array ( 0 => 1, 1 => 10, )
ADDED: key: array ( 0 => 1, 1 => 10, 'key' => array ( ), )
ADDED: 1: array ( 0 => 1, 1 => 10, 'key' => array ( ), )
ADDED: 0: array ( 0 => 1, 1 => 10, 'key' => array ( ), )
UPDATED: $->foo[]: array ( 0 => 1, 1 => 10, 'key' => array ( ), )
UPDATED: $->foo[1]: NULL
UPDATED: $->foo[0]: 1.6E-322
UPDATED: $->foo[key]: array ( 2 => 20, )
UPDATED: $->foo[1]: NULL
UPDATED: $->foo: array ( )
[Script ended normally]
prompt>
--FILE--
<?php

class A {
    public $foo = [];
}

$a = new A;
$handle = phpdbg_watch_recursive('', $a, function($name, $type, $value = null) {
    if ($type == PhpdbgWatchModification::REMOVED) {
        print "$name REMOVED\n";
    } else {
        print "{$type->name}: $name: " . preg_replace("(\s+)", " ", var_export($value, true)) . "\n";
    }
});

$a->foo[] = 1;
$a->foo[1] = 10;
$a->foo += [2];
$a->foo["key"] = [];
$a->foo["key"][2] = 20;
$a->foo = [];

?>
--TEST--
Pdo\Pgsql::setNoticeCallback() use F ZPP for trampoline callback and does not leak
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require_once __DIR__ . "/config.inc";

$db = new Pdo\Pgsql($config['ENV']['PDOTEST_DSN']);

function disp($message) { echo trim($message)."\n"; }
function attach($db, $callback)
{
    if (is_array($callback) && $callback[0] === null) {
        $callback = $callback[1];
        $rc = new ReflectionClass(Pdo\Pgsql::class);
        $db = $rc->newInstanceWithoutConstructor();
    }
    $db->setNoticeCallback($callback);
}

class T { public function z($m) { echo $m."\n"; } public function __call($m, $p) { echo "bah $m\n"; } }
$t = new T;
$rounds = [
    [ $t, 'disp' ],
    3, // Error; but then as the old callback is kept, it will be used in the call that follows the caught error.
    null, // No callback: clear everything.
    'wouldAnyoneNameAFunctionThisWay', // So this one will crash and *no output will follow*.
    [ null, [ $t, 'disp' ] ], // Valid callback on an unvalid Pdo.
    [ $t, 'disp' ],
];
require __DIR__ . '/issue78621.inc';

?>
--EXPECTF--
bah disp
Caught TypeError: %s: Argument #1 ($callback) %s
bah disp
Caught TypeError: %s: Argument #1 ($callback) %s
Caught Error: %s object is uninitialized
bah disp
array(1) {
  [0]=>
  array(1) {
    ["a"]=>
    string(2) "oh"
  }
}
Done


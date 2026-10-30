--TEST--
Pdo\Pgsql::setNoticeCallback()
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
function attach($db, $callback) { $db->setNoticeCallback($callback); }

$rounds = [
    'disp', // Correct.
    3, // Error, so the old callback is kept, and will be used in the call that follows the caught error.
    null, // No callback. Hopefully this clears everything.
    'wouldAnyoneNameAFunctionThatWay', // So this one will crash and *no output will follow*.
];
require __DIR__ . '/issue78621.inc';

?>
--EXPECTF--
NOTICE:  I tampered your data, did you know?
Caught TypeError: %s: Argument #1 ($callback) %s
NOTICE:  I tampered your data, did you know?
Caught TypeError: %s: Argument #1 ($callback) %s
array(1) {
  [0]=>
  array(1) {
    ["a"]=>
    string(2) "oh"
  }
}
Done

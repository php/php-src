--TEST--
pgsqlSetNoticeCallback catches Postgres "raise notice".
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require_once dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
require_once dirname(__FILE__) . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
function disp($message) { echo trim($message)."\n"; }
function attach($db)
{
    $db->pgsqlSetNoticeCallback('disp');
}
require dirname(__FILE__) . '/issue78621.inc';
?>
--EXPECT--
NOTICE:  I tampered your data, did you know?
array(1) {
  [0]=>
  array(1) {
    ["a"]=>
    string(2) "oh"
  }
}
Done

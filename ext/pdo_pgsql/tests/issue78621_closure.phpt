--TEST--
Pdo\Pgsql::setNoticeCallback catches Postgres "raise notice".
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
function attach($db, $prefix = '')
{
    global $flavor;
    switch($flavor)
    {
        case 0:
            $db->setNoticeCallback(function($message) use($prefix) { echo $prefix.trim($message)."\n"; });
            // https://github.com/php/php-src/pull/4823#pullrequestreview-335623806
            $eraseCallbackMemoryHere = (object)[1];
            break;
        case 1:
            $closure = function($message) use($prefix) { echo $prefix.'('.get_class($this).')'.trim($message)."\n"; };
            $db->setNoticeCallback($closure->bindTo(new \stdClass));
            break;
    }
}
echo "Testing with a simple inline closure:\n";
$flavor = 0;
require dirname(__FILE__) . '/issue78621.inc';
echo "Testing with a postbound closure object:\n";
++$flavor;
require dirname(__FILE__) . '/issue78621.inc';
?>
--EXPECT--
Testing with a simple inline closure:
NOTICE:  I tampered your data, did you know?
ReNOTICE:  I tampered your data, did you know?
array(1) {
  [0]=>
  array(1) {
    ["a"]=>
    string(2) "oh"
  }
}
Done
Testing with a postbound closure object:
(stdClass)NOTICE:  I tampered your data, did you know?
Re(stdClass)NOTICE:  I tampered your data, did you know?
array(1) {
  [0]=>
  array(1) {
    ["a"]=>
    string(2) "oh"
  }
}
Done

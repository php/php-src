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
class Logger
{
    public function disp($message) { echo trim($message)."\n"; }
    public function dispRe($message) { echo "Re".trim($message)."\n"; }
    public function __call(string $method, array $args)
    {
        $realMethod = strtr($method, [ 'whatever' => 'disp' ]);
        echo "$method trampoline for $realMethod\n";
        return call_user_func_array([ $this, $realMethod ], $args);
    }
}
$logger = new Logger();
function attach($db, $prefix = '')
{
    global $logger;
    global $flavor;
    switch($flavor)
    {
        case 0: $db->setNoticeCallback([ $logger, 'disp'.$prefix ]); break;
        case 1: $db->setNoticeCallback([ $logger, 'whatever'.$prefix ]); break;
    }
}
echo "Testing with method explicitely plugged:\n";
$flavor = 0;
require dirname(__FILE__) . '/issue78621.inc';
echo "Testing with a bit of magic:\n";
++$flavor;
require dirname(__FILE__) . '/issue78621.inc';
?>
--EXPECT--
Testing with method explicitely plugged:
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
Testing with a bit of magic:
whatever trampoline for disp
NOTICE:  I tampered your data, did you know?
whateverRe trampoline for dispRe
ReNOTICE:  I tampered your data, did you know?
array(1) {
  [0]=>
  array(1) {
    ["a"]=>
    string(2) "oh"
  }
}
Done

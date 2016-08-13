--TEST--
Bug #42802 (Namespace not supported in types)
--FILE--
<?php
namespace foo;

class bar {
}

function test1(bar $bar) {
	echo "ok\n";
}

function test2(\foo\bar $bar) {
        echo "ok\n";
}
function test3(\foo\bar $bar) {
        echo "ok\n";
}
function test4(\Exception $e) {
	echo "ok\n";
}
function test5(\bar $bar) {
        echo "bug\n";
}

$x = new bar();
$y = new \Exception();
test1($x);
test2($x);
test3($x);
test4($y);
test5($x);
--EXPECTF--
ok
ok
ok
ok

Fatal error: Uncaught TypeError: Argument 1 passed to foo\test5() must be an instance of bar, instance of foo\bar given, called in %sbug42802.php on line %d and defined in %sbug42802.php:%d
Stack trace:
#0 %s(%d): foo\test5(Object(foo\bar))
#1 {main}
  thrown in %sbug42802.php on line %d

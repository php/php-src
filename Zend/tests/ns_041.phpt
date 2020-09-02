--TEST--
041: Constants in namespace
--FILE--
<?php
namespace test\ns1;

const FOO = "ok\n";

echo(FOO);
echo(\test\ns1\FOO);
echo(\test\ns1\FOO);
echo(BAR);

const BAR = "ok\n";
?>
--EXPECTF--
ok
ok
ok

Fatal error: Uncaught Error: Undefined constant "test\ns1\BAR" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d

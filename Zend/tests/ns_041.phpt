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

--EXPECTF--
ok
ok
ok

Notice: Use of undefined constant BAR - assumed 'BAR' in %sns_041.php on line 9
BAR

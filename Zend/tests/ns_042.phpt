--TEST--
042: Import in namespace and constants
--FILE--
<?php
namespace test\ns1;

use test\ns1 as ns2;
use test as ns3;

const FOO = "ok\n";


echo FOO;
echo \test\ns1\FOO;
echo \test\ns1\FOO;
echo ns2\FOO;
echo ns3\ns1\FOO;
--EXPECT--
ok
ok
ok
ok
ok

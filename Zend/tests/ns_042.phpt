--TEST--
042: Import in namespace and constants
--FILE--
<?php
namespace test::ns1;

const FOO = "ok\n";

import test::ns1 as ns2;
import test as ns3;

echo FOO;
echo test::ns1::FOO;
echo ::test::ns1::FOO;
echo ns2::FOO;
echo ns3::ns1::FOO;
--EXPECT--
ok
ok
ok
ok
ok

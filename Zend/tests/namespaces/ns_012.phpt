--TEST--
012: Import in namespace and functions
--FILE--
<?php
namespace test\ns1;

function foo() {
  echo __FUNCTION__,"\n";
}

use test\ns1 as ns2;
use test as ns3;

foo();
bar();
\test\ns1\foo();
\test\ns1\bar();
ns2\foo();
ns2\bar();
ns3\ns1\foo();
ns3\ns1\bar();

function bar() {
  echo __FUNCTION__,"\n";
}
?>
--EXPECT--
test\ns1\foo
test\ns1\bar
test\ns1\foo
test\ns1\bar
test\ns1\foo
test\ns1\bar
test\ns1\foo
test\ns1\bar

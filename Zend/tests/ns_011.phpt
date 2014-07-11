--TEST--
011: Function in namespace
--FILE--
<?php
namespace test\ns1;

function foo() {
  echo __FUNCTION__,"\n";
}
  
foo();
\test\ns1\foo();
bar();
\test\ns1\bar();

function bar() {
  echo __FUNCTION__,"\n";
}

--EXPECT--
test\ns1\foo
test\ns1\foo
test\ns1\bar
test\ns1\bar

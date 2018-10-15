--TEST--
038: Name ambiguity (namespace name or internal class name)
--FILE--
<?php
namespace Exception;
function foo() {
  echo "ok\n";
}
\Exception\foo();
\Exception::bar();
--EXPECTF--
ok

Fatal error: Uncaught Error: Call to undefined method Exception::bar() in %sns_038.php:7
Stack trace:
#0 {main}
  thrown in %sns_038.php on line 7

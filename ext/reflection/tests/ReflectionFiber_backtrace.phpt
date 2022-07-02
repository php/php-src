--TEST--
ReflectionFiber backtrace test
--FILE--
<?php

function suspend_fiber(): void {
    Fiber::suspend();
}

class Test
{
    public function __invoke(string $arg): void
    {
        suspend_fiber();
    }
}

$fiber = new Fiber(new Test);

$fiber->start('test');

$reflection = new ReflectionFiber($fiber);

var_dump($reflection->getTrace(DEBUG_BACKTRACE_PROVIDE_OBJECT));

?>
--EXPECTF--
array(3) {
  [0]=>
  array(6) {
    ["file"]=>
    string(%d) "%sReflectionFiber_backtrace.php"
    ["line"]=>
    int(4)
    ["function"]=>
    string(7) "suspend"
    ["class"]=>
    string(5) "Fiber"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(0) {
    }
  }
  [1]=>
  array(4) {
    ["file"]=>
    string(%d) "%sReflectionFiber_backtrace.php"
    ["line"]=>
    int(11)
    ["function"]=>
    string(13) "suspend_fiber"
    ["args"]=>
    array(0) {
    }
  }
  [2]=>
  array(5) {
    ["function"]=>
    string(8) "__invoke"
    ["class"]=>
    string(4) "Test"
    ["object"]=>
    object(Test)#2 (0) {
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(1) {
      [0]=>
      string(4) "test"
    }
  }
}

--TEST--
Pipe operator handles all callable styles
--FILE--
<?php

namespace Beep {
  function test(int $x) {
    print $x;
  }
}

namespace Bar {
    use function \Beep\test;

    5 |> test(...);
}
?>
--EXPECT--
5

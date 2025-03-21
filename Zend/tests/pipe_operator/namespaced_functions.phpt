--TEST--
Pipe operator handles all callable styles
--FILE--
<?php

namespace Beep {
  function test(int $x) {
    echo $x, PHP_EOL;
  }
}

namespace Bar {
    use function \Beep\test;

    5 |> test(...);

    5 |> \Beep\test(...);
}
?>
--EXPECT--
5
5

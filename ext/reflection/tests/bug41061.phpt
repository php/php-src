--TEST--
Reflection Bug #41061 ("visibility error" in ReflectionFunction::__toString())
--FILE--
<?php

function foo() {
}

class bar {
    private function foo() {
    }
}

echo new ReflectionFunction('foo'), "\n";
echo new ReflectionMethod('bar', 'foo'), "\n";
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Function [ <user> function foo ] {
  @@ %sbug41061.php 3 - 4
}

Method [ <user> private method foo ] {
  @@ %sbug41061.php 7 - 8
}

===DONE===

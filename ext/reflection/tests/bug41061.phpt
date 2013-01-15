--TEST--
Reflection Bug #41061 ("visibility error" in ReflectionFunction::export())
--FILE--
<?php

function foo() {
}
 
class bar {
    private function foo() {
    }
}

Reflection::export(new ReflectionFunction('foo'));
Reflection::export(new ReflectionMethod('bar', 'foo'));
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

--TEST--
Bug #75186: Inconsistent reflection of Closure:::__invoke()
--FILE--
<?php

$rc = new ReflectionClass(Closure::class);
foreach ($rc->getMethods() as $method) {
    if ($method->name == '__invoke') {
        var_dump($method);
        $method->invoke(
            function($what) { echo "Hello $what!\n"; },
            "World"
        );
    }
}

?>
--EXPECTF--
object(ReflectionMethod)#%d (2) {
  ["name"]=>
  string(8) "__invoke"
  ["class"]=>
  string(7) "Closure"
}
Hello World!

--TEST--
Cannot directly instantiate Directory class.
--FILE--
<?php

try {
    $d = new Directory();
    var_dump($d);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
object(Directory)#1 (0) {
  ["path"]=>
  uninitialized(string)
  ["handle"]=>
  uninitialized(mixed)
}

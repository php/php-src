--TEST--
GH-19801 (var_dump() memory leak with __debugInfo() that modifies circular references)
--FILE--
<?php

$a = [
    new class {
        function __debugInfo() {
            global $b;
            $b->a = null;
            gc_collect_cycles();
            return [];
        }
    },
];

$b = new stdClass;
$b->a = &$a;

var_dump($b);
?>
--EXPECTF--
object(stdClass)#2 (1) {
  ["a"]=>
  &array(1) {
    [0]=>
    object(class@anonymous)#1 (0) {
    }
  }
}

--TEST--
GH-19801 (debug_zval_dump() leak with __debugInfo() that modifies circular references)
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

debug_zval_dump($b);
?>
--EXPECTF--
object(stdClass)#2 (1) refcount(%d){
  ["a"]=>
  reference refcount(%d) {
    array(1) packed refcount(%d){
      [0]=>
      object(class@anonymous)#1 (0) refcount(%d){
      }
    }
  }
}

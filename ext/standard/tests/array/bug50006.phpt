--TEST--
Bug #50006 (Segfault caused by uksort())
--FILE--
<?php

$data = array(
    'bar-bazbazbaz.' => 0,
    'bar-bazbazbaz-' => 0,
    'foo' => 0,
);
uksort($data, 'magic_sort_cmp');
print_r($data);

function magic_sort_cmp($a, $b) {
  $a = substr($a, 1);
  $b = substr($b, 1);
  if (!$a) return $b ? -1 : 0;
  if (!$b) return 1;
  return magic_sort_cmp($a, $b);
}

?>
--EXPECTF--
Array
(
    [foo] => 0
    [bar-bazbazbaz.] => 0
    [bar-bazbazbaz-] => 0
)

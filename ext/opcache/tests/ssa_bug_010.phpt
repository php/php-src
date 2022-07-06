--TEST--
Wrong assertion
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo($parts, $a, $b) {
  $name = $a;
  $level = 1;
  foreach ($parts as $part) {
    if ($level == 1) {
      $level = 4;
      $found = true;
      switch ($part) {
        case 'general':
          break;
        case 'bg':
          $name = $b;
          break;
      }
      if ($found) {
        continue;
      }
    }
    if ($level == 2) {
      continue;
    }
  }
  return $name;
}
?>
OK
--EXPECT--
OK

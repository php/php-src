--TEST--
Output buffering tests
--POST--
--GET--
--FILE--
<?php
ob_start();
echo ob_get_level();
echo 'A';
  ob_start();
  echo ob_get_level();
  echo 'B';
  $b = ob_get_contents();
  ob_end_clean();
$a = ob_get_contents();
ob_end_clean();
echo $b;
echo $a;
?>
--EXPECT--
2B1A

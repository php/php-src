--TEST--
mb_ereg_replace() compat test 5
--SKIPIF--
extension_loaded('mbstring') or die('skip');
--FILE--
<?php 
/* (counterpart: ext/standard/tests/reg/007.phpt) */
  $a="abcd";
  $b=mb_ereg_replace("abcd","",$a);
  echo "strlen(\$b)=".strlen($b);
?>
--EXPECT--
strlen($b)=0

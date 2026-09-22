--TEST--
mb_ereg_replace() compat test 5
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/007.phpt) */
  $a="abcd";
  $b=mb_ereg_replace("abcd","",$a);
  echo "strlen(\$b)=".strlen($b);
?>
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
strlen($b)=0

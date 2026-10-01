--TEST--
mb_ereg_replace() compat test 4
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/006.phpt) */
  $a="This is a nice and simple string";
  echo mb_ereg_replace("^This","That",$a);
?>
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
That is a nice and simple string

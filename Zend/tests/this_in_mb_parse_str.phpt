--TEST--
$this re-assign in mb_parse_str()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
function foo() {
	mb_parse_str("this=42");
	var_dump($this);
}
foo();
?>
--EXPECTF--
Deprecated: mb_parse_str(): Calling mb_parse_str() without the result argument is deprecated in %s on line %d

Fatal error: Uncaught Error: Cannot re-assign $this in %sthis_in_mb_parse_str.php:3
Stack trace:
#0 %sthis_in_mb_parse_str.php(3): mb_parse_str('this=42')
#1 %sthis_in_mb_parse_str.php(6): foo()
#2 {main}
  thrown in %sthis_in_mb_parse_str.php on line 3

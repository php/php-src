--TEST--
$this re-assign
--FILE--
<?php
function foo() {
	$a = "this";
	$$a = 0;
	var_dump($$a);
}
foo();
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot re-assign $this in %sthis_reassign.php:4
Stack trace:
#0 %sthis_reassign.php(7): foo()
#1 {main}
  thrown in %sthis_reassign.php on line 4

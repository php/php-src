--TEST--
$this re-assign in extract()
--FILE--
<?php
function foo() {
	extract(["this"=>42]);
	var_dump($this);
}
foo();
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot re-assign $this in %sthis_in_extract.php:3
Stack trace:
#0 %sthis_in_extract.php(3): extract(Array)
#1 %sthis_in_extract.php(6): foo()
#2 {main}
  thrown in %sthis_in_extract.php on line 3

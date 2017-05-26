--TEST--
$this re-assign in parse_str()
--FILE--
<?php
function foo() {
	parse_str("this=42");
	var_dump($this);
}
foo();
?>
--EXPECTF--
Deprecated: parse_str(): Calling parse_str() without the result argument is deprecated in %s on line %d

Fatal error: Uncaught Error: Cannot re-assign $this in %sthis_in_parse_str.php:3
Stack trace:
#0 %sthis_in_parse_str.php(3): parse_str('this=42')
#1 %sthis_in_parse_str.php(6): foo()
#2 {main}
  thrown in %sthis_in_parse_str.php on line 3

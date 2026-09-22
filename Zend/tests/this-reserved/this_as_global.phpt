--TEST--
$this as global variable
--FILE--
<?php
function foo() {
    global $this;
    var_dump($this);
}
foo();
?>
--EXPECTF--
Fatal error: Cannot use $this as global variable in %sthis_as_global.php on line 3

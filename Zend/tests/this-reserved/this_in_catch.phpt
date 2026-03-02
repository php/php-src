--TEST--
$this in catch
--FILE--
<?php
class C {
    function foo() {
        try {
            throw new Exception();
        } catch (Exception $this) {
        }
        var_dump($this);
    }
}
$obj = new C;
$obj->foo();
?>
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_catch.php on line 6

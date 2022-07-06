--TEST--
Bug #55719 (Argument restriction should come with a more specific error message)
--FILE--
<?php
class Sub implements ArrayAccess {
    public function offsetSet() {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of Sub::offsetSet() must be compatible with ArrayAccess::offsetSet(mixed $offset, mixed $value) in %sargument_restriction_005.php on line %d

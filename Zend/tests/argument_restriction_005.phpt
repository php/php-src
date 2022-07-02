--TEST--
Bug #55719 (Argument restriction should come with a more specific error message)
--FILE--
<?php
class Sub implements ArrayAccess {
    public function offsetSet(): void {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of Sub::offsetSet(): void must be compatible with ArrayAccess::offsetSet(mixed $offset, mixed $value): void in %sargument_restriction_005.php on line %d

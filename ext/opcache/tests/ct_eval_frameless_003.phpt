--TEST--
Test ct eval of frameless function
--EXTENSIONS--
opcache
--FILE--
<?php
class Foo {
    public function __toString() {
        return strpos('foo', 'o', 0);
    }
}
echo (string)new Foo();
?>
--EXPECT--
1

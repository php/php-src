--TEST--
Diamond: generic class forwarding the same T into two paths is accepted
--FILE--
<?php
interface Box<T> {}
interface Wrapper<U> extends Box<U> {}

class C<T> implements Wrapper<T>, Box<T> {}
echo "OK\n";
?>
--EXPECT--
OK

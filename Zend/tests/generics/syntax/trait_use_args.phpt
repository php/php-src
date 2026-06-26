--TEST--
Generic syntax: trait use with type arguments
--FILE--
<?php
trait Container<T> {
    public T $tval;
}
class Box<T : object> {
    use Container<T>;
}
$r = new ReflectionClass('Box');
echo $r->isGeneric() ? 'ok' : 'fail', "\n";
?>
--EXPECT--
ok

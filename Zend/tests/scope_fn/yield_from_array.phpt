--TEST--
Scope-fn generator: yield from another iterable
--FILE--
<?php
function outer() {
    $items = ['a', 'b', 'c'];
    $fn = fn() {
        yield 'start';
        yield from $items;
        yield 'end';
    };
    foreach ($fn() as $v) {
        echo "got: $v\n";
    }
}
outer();
echo "done\n";
?>
--EXPECT--
got: start
got: a
got: b
got: c
got: end
done

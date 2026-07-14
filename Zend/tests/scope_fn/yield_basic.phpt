--TEST--
Scope function with yield: basic generator behavior, accessing parent CVs
--FILE--
<?php
function outer() {
    $x = 10;
    $gen = fn() {
        yield $x;
        $x++;
        yield $x;
        $x++;
        yield $x;
    };
    $g = $gen();
    foreach ($g as $v) {
        echo "got: $v\n";
    }
    echo "x after: $x\n";
}
outer();
echo "done\n";
?>
--EXPECT--
got: 10
got: 11
got: 12
x after: 12
done

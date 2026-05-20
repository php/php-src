--TEST--
Variable variables ($$name) inside a scope-fn body resolve against the parent's CVs
--FILE--
<?php
function outer() {
    $a = 1;
    $b = 2;
    $name = 'a';

    /* Read via varvar: dereferences $name (parent CV) to "a", then reads
     * parent CV $a. */
    $read = fn() { return $$name; };
    var_dump($read()); // int(1)

    /* Write via varvar: must observably mutate the parent CV. */
    $write = fn() { $$name = 99; };
    $write();
    var_dump($a); // int(99)

    /* Re-target by mutating the parent CV $name and re-reading. */
    $name = 'b';
    var_dump($read()); // int(2)

    /* Materializing a previously-undefined parent CV via varvar write. */
    $create = fn() { $$name = 'created'; };
    $name = 'fresh';
    $create();
    var_dump($fresh); // string(7) "created"
}
outer();
echo "done\n";
?>
--EXPECT--
int(1)
int(99)
int(2)
string(7) "created"
done

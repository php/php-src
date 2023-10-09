--TEST--
Bug GH-9500: Disjunctive Normal Form Types - readonly property followed by (
--FILE--
<?php
class Dnf
{
    var A|(B&C) $a;
    var (B&C)|A $b;
    private A|(B&C) $c;
    private (B&C)|A $d;
    static A|(B&C) $e;
    static (B&C)|A $f;
    private static A|(B&C) $g;
    private static (B&C)|A $h;
    readonly private A|(B&C) $i;
    readonly private (B&C)|A $j;
    readonly A|(B&C) $k;
    readonly (B&C)|A $l;
    private readonly A|(B&C) $m;
    private readonly (B&C)|A $n;
}
?>
DONE
--EXPECT--
DONE

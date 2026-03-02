--TEST--
Readonly match of imported trait properties (valid)
--FILE--
<?php

trait T1 {
    public readonly int $prop;
}
trait T2 {
    public readonly int $prop;
}
class C {
    use T1, T2;
}

?>
===DONE===
--EXPECT--
===DONE===

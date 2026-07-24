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

trait TDefault1 {
    public readonly int $prop = 1;
}
trait TDefault2 {
    public readonly int $prop = 1;
}
class CDefault {
    use TDefault1, TDefault2;
}

var_dump(new CDefault()->prop);

?>
===DONE===
--EXPECT--
int(1)
===DONE===

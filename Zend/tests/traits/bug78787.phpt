--TEST--
Bug #78787: Segfault with trait overriding inherited private shadow property
--FILE--
<?php

trait T {
    private $prop;
}
class C1 {
    /** Doc comment */
    private $prop;
}
class C2 extends C1 {
}
class C3 extends C2 {
    use T;
}

?>
===DONE===
--EXPECT--
===DONE===

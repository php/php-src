--TEST--
Intersection type and by-ref parameter parsing with comments
--FILE--
<?php

class Test {
    function f1(A & /*
        Comment // ** / / * * **/ B $p) {}
    function f2(A & // Comment
        B $p) {}
    function f3(A & # Comment
        B $p) {}
    function f4(A & #
        B $p) {}
    function f6(A & /*
        Comment // ** / / * * **/ $p) {}
    function f7(A & // Comment
        $p) {}
    function f8(A & # Comment
        $p) {}
    function f9(A & #
        $p) {}
}

?>
--EXPECT--

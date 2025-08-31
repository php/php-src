--TEST--
Behavior of static variables in trait methods
--FILE--
<?php

trait T {
    public static function counter() {
        static $i = 0;
        var_dump(++$i);
    }
}

class C1 {
    use T {
        T::counter as counter1;
        T::counter as counter2;
    }
}

class C2 {
    use T;
}

C1::counter();
C1::counter1();
C1::counter2();
C2::counter();

C1::counter();
C1::counter1();
C1::counter2();
C2::counter();

?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
int(2)
int(2)
int(2)
int(2)

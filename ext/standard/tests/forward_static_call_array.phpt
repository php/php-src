--TEST--
mixed forward_static_call_array ( callable $function , array $parameters );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
    die('SKIP php version so lower.');
}
?>
--FILE--
<?php

function test() {
    $args = func_get_args();
    echo "C " . join(',', $args) . " \n";
}

class A {

    const NAME = 'A';

    public static function test() {
        $args = func_get_args();
        echo static::NAME, " " . join(',', $args) . " \n";
    }

}

class B extends A {

    const NAME = 'B';

    public static function test() {
        echo self::NAME, "\n";
        forward_static_call_array(array('A', 'test'), array('more', 'args'));
        forward_static_call_array('test', array('other', 'args'));
    }

}

B::test('foo');
?>
--EXPECT--
B
B more,args 
C other,args

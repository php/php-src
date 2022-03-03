--TEST--
Various pow() tests
--FILE--
<?php

define('LONG_MAX', is_int(5000000000)? 9223372036854775807 : 0x7FFFFFFF);
define('LONG_MIN', -LONG_MAX - 1);
printf("%d,%d,%d,%d\n",is_int(LONG_MIN  ),is_int(LONG_MAX  ),
                       is_int(LONG_MIN-1),is_int(LONG_MAX+1));

function epsilon_equal($left, $right): bool {
    return abs(($left-$right) / $left) < 1e-12;
}

var_dump(0.25 === pow(-2,-2));
var_dump(-0.5 === pow(-2,-1));
var_dump(1    === pow(-2, 0));
var_dump(-2   === pow(-2, 1));
var_dump(4    === pow(-2, 2));
var_dump(1.0  === pow(-1,-2));
var_dump(-1.0 === pow(-1,-1));
var_dump(1    === pow(-1, 0));
var_dump(-1   === pow(-1, 1));
var_dump(1    === pow(-1, 2));
var_dump(TRUE === is_infinite(pow(0,-2)));
var_dump(TRUE === is_infinite(pow(0,-1)));
var_dump(1    === pow( 0, 0));
var_dump(0    === pow( 0, 1));
var_dump(0    === pow( 0, 2));
var_dump(1.0  === pow( 1,-2));
var_dump(1.0  === pow( 1,-1));
var_dump(1    === pow( 1, 0));
var_dump(1    === pow( 1, 1));
var_dump(1    === pow( 1, 2));
var_dump(0.25 === pow( 2,-2));
var_dump(0.5  === pow( 2,-1));
var_dump(1    === pow( 2, 0));
var_dump(2    === pow( 2, 1));
var_dump(4    === pow( 2, 2));
var_dump(0.25 === pow(-2,-2.0));
var_dump(-0.5 === pow(-2,-1.0));
var_dump(1.0  === pow(-2, 0.0));
var_dump(-2.0 === pow(-2, 1.0));
var_dump(4.0  === pow(-2, 2.0));
var_dump(1.0  === pow(-1,-2.0));
var_dump(-1.0 === pow(-1,-1.0));
var_dump(1.0  === pow(-1, 0.0));
var_dump(-1.0 === pow(-1, 1.0));
var_dump(1.0  === pow(-1, 2.0));
var_dump(TRUE === is_infinite(pow(0,-2.0)));
var_dump(TRUE === is_infinite(pow(0,-1.0)));
var_dump(1.0  === pow( 0, 0.0));
var_dump(0.0  === pow( 0, 1.0));
var_dump(0.0  === pow( 0, 2.0));
var_dump(1.0  === pow( 1,-2.0));
var_dump(1.0  === pow( 1,-1.0));
var_dump(1.0  === pow( 1, 0.0));
var_dump(1.0  === pow( 1, 1.0));
var_dump(1.0  === pow( 1, 2.0));
var_dump(0.25 === pow( 2,-2.0));
var_dump(0.5  === pow( 2,-1.0));
var_dump(1.0  === pow( 2, 0.0));
var_dump(2.0  === pow( 2, 1.0));
var_dump(4.0  === pow( 2, 2.0));
var_dump(2147483648 === pow(2,31));
var_dump(epsilon_equal( -2147483648, pow(-2,31) ));
var_dump(1000000000 === pow(10,9));
var_dump(100000000  === pow(-10,8));
var_dump(1    === pow(-1,1443279822));
var_dump(-1   === pow(-1,1443279821));
var_dump(epsilon_equal( sqrt(2), pow(2,1/2) ));
var_dump(0.25 === pow(-2.0,-2.0));
var_dump(-0.5 === pow(-2.0,-1.0));
var_dump(1.0  === pow(-2.0, 0.0));
var_dump(-2.0 === pow(-2.0, 1.0));
var_dump(4.0  === pow(-2.0, 2.0));
var_dump(1.0  === pow(-1.0,-2.0));
var_dump(-1.0 === pow(-1.0,-1.0));
var_dump(1.0  === pow(-1.0, 0.0));
var_dump(-1.0 === pow(-1.0, 1.0));
var_dump(1.0  === pow(-1.0, 2.0));
var_dump(TRUE === is_infinite(pow(0.0,-2.0)));
var_dump(TRUE === is_infinite(pow(0.0,-1.0)));
var_dump(1.0  === pow( 0.0, 0.0));
var_dump(0.0  === pow( 0.0, 1.0));
var_dump(0.0  === pow( 0.0, 2.0));
var_dump(1.0  === pow( 1.0,-2.0));
var_dump(1.0  === pow( 1.0,-1.0));
var_dump(1.0  === pow( 1.0, 0.0));
var_dump(1.0  === pow( 1.0, 1.0));
var_dump(1.0  === pow( 1.0, 2.0));
var_dump(0.25 === pow( 2.0,-2.0));
var_dump(0.5  === pow( 2.0,-1.0));
var_dump(1.0  === pow( 2.0, 0.0));
var_dump(2.0  === pow( 2.0, 1.0));
var_dump(4.0  === pow( 2.0, 2.0));
var_dump(0.25 === pow(-2.0,-2));
var_dump(-0.5 === pow(-2.0,-1));
var_dump(1.0  === pow(-2.0, 0));
var_dump(-2.0 === pow(-2.0, 1));
var_dump(4.0  === pow(-2.0, 2));
var_dump(1.0  === pow(-1.0,-2));
var_dump(-1.0 === pow(-1.0,-1));
var_dump(1.0  === pow(-1.0, 0));
var_dump(-1.0 === pow(-1.0, 1));
var_dump(1.0  === pow(-1.0, 2));
var_dump(TRUE === is_infinite(pow( 0.0,-2)));
var_dump(TRUE === is_infinite(pow( 0.0,-1)));
var_dump(1.0  === pow( 0.0, 0));
var_dump(0.0  === pow( 0.0, 1));
var_dump(0.0  === pow( 0.0, 2));
var_dump(1.0  === pow( 1.0,-2));
var_dump(1.0  === pow( 1.0,-1));
var_dump(1.0  === pow( 1.0, 0));
var_dump(1.0  === pow( 1.0, 1));
var_dump(1.0  === pow( 1.0, 2));
var_dump(0.25 === pow( 2.0,-2));
var_dump(0.5  === pow( 2.0,-1));
var_dump(1.0  === pow( 2.0, 0));
var_dump(2.0  === pow( 2.0, 1));
var_dump(4.0  === pow( 2.0, 2));
var_dump(2.0  === pow(   4, 0.5));
var_dump(2.0  === pow( 4.0, 0.5));
var_dump(3.0  === pow(  27, 1/3));
var_dump(3.0  === pow(27.0, 1/3));
var_dump(0.5  === pow(   4, -0.5));
var_dump(0.5  === pow( 4.0, -0.5));
var_dump(LONG_MAX-1 === pow(LONG_MAX-1,1));
var_dump(LONG_MIN+1 === pow(LONG_MIN+1,1));
var_dump(epsilon_equal( (LONG_MAX-1)*(LONG_MAX-1) , pow(LONG_MAX-1,2) ));
var_dump(epsilon_equal( (LONG_MIN+1)*(LONG_MIN+1) , pow(LONG_MIN+1,2) ));
var_dump((float)(LONG_MAX-1) === pow(LONG_MAX-1,1.0));
var_dump((float)(LONG_MIN+1) === pow(LONG_MIN+1,1.0));
var_dump(epsilon_equal( (LONG_MAX-1)*(LONG_MAX-1) , pow(LONG_MAX-1,2.0) ));
var_dump(epsilon_equal( (LONG_MIN+1)*(LONG_MIN+1) , pow(LONG_MIN+1,2.0) ));
var_dump(LONG_MAX === pow(LONG_MAX,1));
var_dump(LONG_MIN === pow(LONG_MIN,1));
var_dump(epsilon_equal( LONG_MAX*LONG_MAX , pow(LONG_MAX,2) ));
var_dump(epsilon_equal( LONG_MIN*LONG_MIN , pow(LONG_MIN,2) ));
var_dump((float)LONG_MAX === pow(LONG_MAX,1.0));
var_dump((float)LONG_MIN === pow(LONG_MIN,1.0));
var_dump(epsilon_equal( LONG_MAX*LONG_MAX , pow(LONG_MAX,2.0) ));
var_dump(epsilon_equal( LONG_MIN*LONG_MIN , pow(LONG_MIN,2.0) ));

?>
--EXPECT--
1,1,0,0
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

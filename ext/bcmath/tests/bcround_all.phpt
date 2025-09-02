--TEST--
bcround() function all modes
--EXTENSIONS--
bcmath
--FILE--
<?php
foreach (RoundingMode::cases() as $mode) {
     foreach ([
         '1.0',
         '-1.0',
         '1.2',
         '-1.2',
         '1.7',
         '-1.7',
         '1.5',
         '-1.5',
         '2.5',
         '-2.5',
     ] as $number) {
         printf("%-20s: %s -> %s\n", $mode->name, $number, bcround($number, 0, $mode));
     }
 }
?>
--EXPECT--
HalfAwayFromZero    : 1.0 -> 1
HalfAwayFromZero    : -1.0 -> -1
HalfAwayFromZero    : 1.2 -> 1
HalfAwayFromZero    : -1.2 -> -1
HalfAwayFromZero    : 1.7 -> 2
HalfAwayFromZero    : -1.7 -> -2
HalfAwayFromZero    : 1.5 -> 2
HalfAwayFromZero    : -1.5 -> -2
HalfAwayFromZero    : 2.5 -> 3
HalfAwayFromZero    : -2.5 -> -3
HalfTowardsZero     : 1.0 -> 1
HalfTowardsZero     : -1.0 -> -1
HalfTowardsZero     : 1.2 -> 1
HalfTowardsZero     : -1.2 -> -1
HalfTowardsZero     : 1.7 -> 2
HalfTowardsZero     : -1.7 -> -2
HalfTowardsZero     : 1.5 -> 1
HalfTowardsZero     : -1.5 -> -1
HalfTowardsZero     : 2.5 -> 2
HalfTowardsZero     : -2.5 -> -2
HalfEven            : 1.0 -> 1
HalfEven            : -1.0 -> -1
HalfEven            : 1.2 -> 1
HalfEven            : -1.2 -> -1
HalfEven            : 1.7 -> 2
HalfEven            : -1.7 -> -2
HalfEven            : 1.5 -> 2
HalfEven            : -1.5 -> -2
HalfEven            : 2.5 -> 2
HalfEven            : -2.5 -> -2
HalfOdd             : 1.0 -> 1
HalfOdd             : -1.0 -> -1
HalfOdd             : 1.2 -> 1
HalfOdd             : -1.2 -> -1
HalfOdd             : 1.7 -> 2
HalfOdd             : -1.7 -> -2
HalfOdd             : 1.5 -> 1
HalfOdd             : -1.5 -> -1
HalfOdd             : 2.5 -> 3
HalfOdd             : -2.5 -> -3
TowardsZero         : 1.0 -> 1
TowardsZero         : -1.0 -> -1
TowardsZero         : 1.2 -> 1
TowardsZero         : -1.2 -> -1
TowardsZero         : 1.7 -> 1
TowardsZero         : -1.7 -> -1
TowardsZero         : 1.5 -> 1
TowardsZero         : -1.5 -> -1
TowardsZero         : 2.5 -> 2
TowardsZero         : -2.5 -> -2
AwayFromZero        : 1.0 -> 1
AwayFromZero        : -1.0 -> -1
AwayFromZero        : 1.2 -> 2
AwayFromZero        : -1.2 -> -2
AwayFromZero        : 1.7 -> 2
AwayFromZero        : -1.7 -> -2
AwayFromZero        : 1.5 -> 2
AwayFromZero        : -1.5 -> -2
AwayFromZero        : 2.5 -> 3
AwayFromZero        : -2.5 -> -3
NegativeInfinity    : 1.0 -> 1
NegativeInfinity    : -1.0 -> -1
NegativeInfinity    : 1.2 -> 1
NegativeInfinity    : -1.2 -> -2
NegativeInfinity    : 1.7 -> 1
NegativeInfinity    : -1.7 -> -2
NegativeInfinity    : 1.5 -> 1
NegativeInfinity    : -1.5 -> -2
NegativeInfinity    : 2.5 -> 2
NegativeInfinity    : -2.5 -> -3
PositiveInfinity    : 1.0 -> 1
PositiveInfinity    : -1.0 -> -1
PositiveInfinity    : 1.2 -> 2
PositiveInfinity    : -1.2 -> -1
PositiveInfinity    : 1.7 -> 2
PositiveInfinity    : -1.7 -> -1
PositiveInfinity    : 1.5 -> 2
PositiveInfinity    : -1.5 -> -1
PositiveInfinity    : 2.5 -> 3
PositiveInfinity    : -2.5 -> -2

--TEST--
DateInterval::format(), except %a
--DESCRIPTION--
%a is covered in a separate test.
Don't want an XFAIL here to cause confusion if a real bug comes up.
--CREDITS--
Daniel Convissor <danielc@php.net>
# TestFest 2010 BKTK
--SKIPIF--
<?php if (!method_exists('DateInterval', 'format')) die("skip: method doesn't exist"); ?>
--FILE--
<?php
date_default_timezone_set('UTC');

$date1 = new DateTime('2000-01-01 00:00:00');
$date2 = new DateTime('2001-03-04 04:05:06');

$interval = $date1->diff($date2);

echo $interval->format('Y=%Y') . "\n";
echo $interval->format('M=%M') . "\n";
echo $interval->format('D=%D') . "\n";
echo $interval->format('H=%H') . "\n";
echo $interval->format('I=%I') . "\n";
echo $interval->format('S=%S') . "\n";
echo $interval->format('R=%R') . "\n";

echo $interval->format('y=%y') . "\n";
echo $interval->format('m=%m') . "\n";
echo $interval->format('d=%d') . "\n";
echo $interval->format('h=%h') . "\n";
echo $interval->format('i=%i') . "\n";
echo $interval->format('s=%s') . "\n";
echo $interval->format('r=%r') . "\n";

echo "\n";

$interval = $date2->diff($date1);

echo $interval->format('inverted R=%R') . "\n";
echo $interval->format('inverted r=%r') . "\n";

echo "\n";

echo $interval->format('%=%%') . "\n";

// Invalid valid format character does not raise warnings.
echo $interval->format('x=%x') . "\n";

?>
--EXPECT--
Y=01
M=02
D=03
H=04
I=05
S=06
R=+
y=1
m=2
d=3
h=4
i=5
s=6
r=

inverted R=-
inverted r=-

%=%
x=%x

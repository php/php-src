--TEST--
Test vprintf() function : usage variations - unsigned formats with unsigned values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/*
 * Test vprintf() when different unsigned formats and unsigned values
 * are passed to the '$format' and '$args' arguments of the function
*/

echo "*** Testing vprintf() : unsigned formats and unsigned values ***\n";

// defining array of unsigned formats
$formats = [
  '"%u" "%+u" "%-u"',
  '"%lu" "%4u" "%-4u"',
  ' "%10.4u" "%-10.4u" "%.4u"',
  '"%\'#2u "%\'2u" "%\'$2u" "%\'_2u"',
  '"%3$u" "%4$u" "%1$u" "%2$u"',
];

// Arrays of unsigned values for the format defined in $format.
// Each sub array contains unsigned values which correspond to each format string in $format
$args_array = array(
  array(1234567, 01234567, 0 ),
  array(12345678900, 1234, 12345),
  array("1234000", 10.1234567e10, 1.2e2),
  array(1, 0, 00, "10_"),
  array(3, 4, 1, 2)
);

// looping to test vprintf() with different unsigned formats from the above $format array
// and with signed and other types of  values from the above $args_array array
$counter = 1;
foreach($formats as $format) {
    echo "\n-- Iteration $counter --\n";
    ob_start();
    $bytes = vprintf($format, $args_array[$counter-1]);
    var_dump(ob_get_clean());
    var_dump($bytes);
    $counter++;
}

?>
--EXPECTF--
*** Testing vprintf() : unsigned formats and unsigned values ***

-- Iteration 1 --
string(22) ""1234567" "342391" "0""
int(22)

-- Iteration 2 --
string(%d) "
Deprecated: Implicit conversion from non-compatible float 12345678900 to int in %s on line %d
"3755744308" "1234" "12345""
int(27)

-- Iteration 3 --
string(%d) "
Deprecated: Implicit conversion from non-compatible float 101234567000 to int in %s on line %d
 "   1234000" "2450319192" "120""
int(32)

-- Iteration 4 --
string(17) ""#1 "0" "$0" "10""
int(17)

-- Iteration 5 --
string(15) ""1" "2" "3" "4""
int(15)

--TEST--
Test money_format() function : basic functionality using national currency symbols
--SKIPIF--
<?php
	if (!function_exists('money_format')) {
		die("SKIP money_format - not supported\n");
	}
?>
--FILE--
<?php
/* Prototype  : string money_format  ( string $format  , float $number  )
 * Description: Formats a number as a currency string
 * Source code: ext/standard/string.c
*/

// ===========================================================================================
// = We do not test for exact return-values, as those might be different between OS-versions =
// ===========================================================================================

echo "*** Testing money_format() : basic functionality***\n";

$value = 1234.5678;
$negative_value = -1234.5678;

// Format with 14 positions of width, 8 digits of
// left precision, 2 of right precision using national
// format for en_US
echo "Format values with 14 positions, 8 digits to left, 2 to right using national format\n";
echo gettype(money_format('%14#8.2n', $value))."\n";
echo gettype(money_format('%14#8.2n', $negative_value))."\n";

// Same again but use '(' for negative values
echo "Format again but with ( for negative values\n";
echo gettype(money_format('%(14#8.2n', $value))."\n";
echo gettype(money_format('%(14#8.2n', $negative_value))."\n";

// Same again but use a '0' for padding character
echo "Format with 0 for padding character\n";
echo gettype(money_format('%=014#8.2n', $value))."\n";
echo gettype(money_format('%=014#8.2n', $negative_value))."\n";

// Same again but use a '*' for padding character
echo "Format again with * for padding character\n";
echo gettype(money_format('%=*14#8.2n', $value))."\n";
echo gettype(money_format('%=*14#8.2n', $negative_value))."\n";

// Same again but disable grouping character
echo "Format again but disable grouping character\n";
echo gettype(money_format('%=*^14#8.2n', $value))."\n";
echo gettype(money_format('%=*^14#8.2n', $negative_value))."\n";

// Same again but suppress currency symbol
echo "Format again suppress currency symbol\n";
echo gettype(money_format('%=*!14#8.2n', $value))."\n";
echo gettype(money_format('%=*!14#8.2n', $negative_value))."\n";

?>
===DONE===
--EXPECTF--
*** Testing money_format() : basic functionality***
Format values with 14 positions, 8 digits to left, 2 to right using national format

Deprecated: Function money_format() is deprecated in %s on line %d
string

Deprecated: Function money_format() is deprecated in %s on line %d
string
Format again but with ( for negative values

Deprecated: Function money_format() is deprecated in %s on line %d
string

Deprecated: Function money_format() is deprecated in %s on line %d
string
Format with 0 for padding character

Deprecated: Function money_format() is deprecated in %s on line %d
string

Deprecated: Function money_format() is deprecated in %s on line %d
string
Format again with * for padding character

Deprecated: Function money_format() is deprecated in %s on line %d
string

Deprecated: Function money_format() is deprecated in %s on line %d
string
Format again but disable grouping character

Deprecated: Function money_format() is deprecated in %s on line %d
string

Deprecated: Function money_format() is deprecated in %s on line %d
string
Format again suppress currency symbol

Deprecated: Function money_format() is deprecated in %s on line %d
string

Deprecated: Function money_format() is deprecated in %s on line %d
string
===DONE===

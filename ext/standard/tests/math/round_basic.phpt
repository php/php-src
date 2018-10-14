--TEST--
Test round() - basic function test for round()
--INI--
precision=14
--FILE--
<?php
/* Prototype  : float round  ( float $val  [, int $precision  ] )
 * Description: Returns the rounded value of val  to specified precision (number of digits
 * after the decimal point)
 * Source code: ext/standard/math.c
 */

echo "*** Testing round() : basic functionality ***\n";

$values = array(123456789,
				123.456789,
				-4.5679123,
				1.23E4,
				-4.567E3,
				0x234567,
				067777777,
				"1.234567",
				"2.3456789e8");

$precision = array(2,
				8,
				0x3,
				04,
				3.6,
				"2",
				"04",
				"3.6",
				"2.1e1",
				null,
				true,
				false);

for ($i = 0; $i < count($values); $i++) {
	echo "round: $values[$i]\n";
	for ($j = 0; $j < count($precision); $j++) {
		$res = round($values[$i], $precision[$j]);
		echo "...with precision $precision[$j]-> ";
		var_dump($res);
	}
}
?>
===Done===
--EXPECTF--
*** Testing round() : basic functionality ***
round: 123456789
...with precision 2-> float(123456789)
...with precision 8-> float(123456789)
...with precision 3-> float(123456789)
...with precision 4-> float(123456789)
...with precision 3.6-> float(123456789)
...with precision 2-> float(123456789)
...with precision 04-> float(123456789)
...with precision 3.6-> float(123456789)
...with precision 2.1e1-> float(123456789)
...with precision -> float(123456789)
...with precision 1-> float(123456789)
...with precision -> float(123456789)
round: 123.456789
...with precision 2-> float(123.46)
...with precision 8-> float(123.456789)
...with precision 3-> float(123.457)
...with precision 4-> float(123.4568)
...with precision 3.6-> float(123.457)
...with precision 2-> float(123.46)
...with precision 04-> float(123.4568)
...with precision 3.6-> float(123.457)
...with precision 2.1e1-> float(123.456789)
...with precision -> float(123)
...with precision 1-> float(123.5)
...with precision -> float(123)
round: -4.5679123
...with precision 2-> float(-4.57)
...with precision 8-> float(-4.5679123)
...with precision 3-> float(-4.568)
...with precision 4-> float(-4.5679)
...with precision 3.6-> float(-4.568)
...with precision 2-> float(-4.57)
...with precision 04-> float(-4.5679)
...with precision 3.6-> float(-4.568)
...with precision 2.1e1-> float(-4.5679123)
...with precision -> float(-5)
...with precision 1-> float(-4.6)
...with precision -> float(-5)
round: 12300
...with precision 2-> float(12300)
...with precision 8-> float(12300)
...with precision 3-> float(12300)
...with precision 4-> float(12300)
...with precision 3.6-> float(12300)
...with precision 2-> float(12300)
...with precision 04-> float(12300)
...with precision 3.6-> float(12300)
...with precision 2.1e1-> float(12300)
...with precision -> float(12300)
...with precision 1-> float(12300)
...with precision -> float(12300)
round: -4567
...with precision 2-> float(-4567)
...with precision 8-> float(-4567)
...with precision 3-> float(-4567)
...with precision 4-> float(-4567)
...with precision 3.6-> float(-4567)
...with precision 2-> float(-4567)
...with precision 04-> float(-4567)
...with precision 3.6-> float(-4567)
...with precision 2.1e1-> float(-4567)
...with precision -> float(-4567)
...with precision 1-> float(-4567)
...with precision -> float(-4567)
round: 2311527
...with precision 2-> float(2311527)
...with precision 8-> float(2311527)
...with precision 3-> float(2311527)
...with precision 4-> float(2311527)
...with precision 3.6-> float(2311527)
...with precision 2-> float(2311527)
...with precision 04-> float(2311527)
...with precision 3.6-> float(2311527)
...with precision 2.1e1-> float(2311527)
...with precision -> float(2311527)
...with precision 1-> float(2311527)
...with precision -> float(2311527)
round: 14680063
...with precision 2-> float(14680063)
...with precision 8-> float(14680063)
...with precision 3-> float(14680063)
...with precision 4-> float(14680063)
...with precision 3.6-> float(14680063)
...with precision 2-> float(14680063)
...with precision 04-> float(14680063)
...with precision 3.6-> float(14680063)
...with precision 2.1e1-> float(14680063)
...with precision -> float(14680063)
...with precision 1-> float(14680063)
...with precision -> float(14680063)
round: 1.234567
...with precision 2-> float(1.23)
...with precision 8-> float(1.234567)
...with precision 3-> float(1.235)
...with precision 4-> float(1.2346)
...with precision 3.6-> float(1.235)
...with precision 2-> float(1.23)
...with precision 04-> float(1.2346)
...with precision 3.6-> float(1.235)
...with precision 2.1e1-> float(1.234567)
...with precision -> float(1)
...with precision 1-> float(1.2)
...with precision -> float(1)
round: 2.3456789e8
...with precision 2-> float(234567890)
...with precision 8-> float(234567890)
...with precision 3-> float(234567890)
...with precision 4-> float(234567890)
...with precision 3.6-> float(234567890)
...with precision 2-> float(234567890)
...with precision 04-> float(234567890)
...with precision 3.6-> float(234567890)
...with precision 2.1e1-> float(234567890)
...with precision -> float(234567890)
...with precision 1-> float(234567890)
...with precision -> float(234567890)
===Done===

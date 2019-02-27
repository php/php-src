--TEST--
Equality of compare operations in compile and run time
--FILE--
<?php
$input = [
	0,
	1,
	2,
	-1,
	2.0,
	2.1,
	-2.0,
	-2.1,
	PHP_INT_MAX,
	PHP_INT_MIN,
	PHP_INT_MAX * 2,
	PHP_INT_MIN * 2,
	INF,
	NAN,
	[],
	[1, 2],
	[1, 2, 3],
	[1 => 2, 0 => 1],
	[1, 'a' => 2],
	[1, 4],
	[1, 'a'],
	[1, 2 => 2],
	[1, [ 2 ]],
	null,
	false,
	true,
	"",
	" ",
	"banana",
	"Banana",
	"banan",
	"0",
	"200",
	"20",
	"20a",
	" \t\n\r\v\f20",
	"2e1",
	"2e150",
	"9179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368",
	"-9179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368",
	"0.1",
	"-0.1",
	"1e-1",
	"-20",
	"-20.0",
	"0x14",
	(string) PHP_INT_MAX * 2,
	(string) PHP_INT_MIN * 2,
];

function makeParam($param){
	return str_replace("\n", "", var_export($param, true));
}

$c = 0;
$f = 0;

function prepareLine($op1, $op2, $cmp, $type){
	global $c;
	$c++;
	$cmp = (int) $cmp;           // runtime compare result (bool)
	$op1_p = makeParam($op1);    // value of first operand as string, e.g. "array (  0 => 1,  2 => 2,)"
	$op2_p = makeParam($op2);    // value of second operand as string
	                             // $type is operation, e.g. '===', '>=' etc.
	/*
	 * Returns code snippet for writing into temp file
	 *
	 * For example:
	 *
	 * if (1 !== (int)(array (  0 => 1,  2 => 2,) >= '-0.1')) {
	 *     echo "array (  0 => 1,  2 => 2,) >= '-0.1'\n";
	 *     $f++;
	 * }
	 * $c--;
	 */

	return <<<EOT
if ($cmp !== (int)($op1_p $type $op2_p)) {
	echo "$op1_p $type $op2_p\\n";
	\$f++;
}
\$c--;

EOT;
}

$filename = __DIR__ . DIRECTORY_SEPARATOR . 'compare_equality_temp.php';
$file = fopen($filename, "w");

fwrite($file, "<?php\n");
$var_cnt = count($input);

foreach ($input as $var1) {
	foreach ($input as $var2) {
		fwrite($file, prepareLine($var1, $var2, $var1 ==  $var2, '=='));
		fwrite($file, prepareLine($var1, $var2, $var1 !=  $var2, '!='));
		fwrite($file, prepareLine($var1, $var2, $var1 === $var2, '==='));
		fwrite($file, prepareLine($var1, $var2, $var1 !== $var2, '!=='));
		fwrite($file, prepareLine($var1, $var2, $var1 >   $var2, '>'));
		fwrite($file, prepareLine($var1, $var2, $var1 >=  $var2, '>='));
		fwrite($file, prepareLine($var1, $var2, $var1 <   $var2, '<'));
		fwrite($file, prepareLine($var1, $var2, $var1 <=  $var2, '<='));
		fwrite($file, prepareLine($var1, $var2, $var1 <=> $var2, '<=>'));
	}
}

fclose($file);

include($filename);

if ($c !== 0) {
	echo "The number of checks does not match: $c\n";
} else {
	echo "Failed: $f\n";
}
?>
===DONE===
--CLEAN--
<?php
$fl = __DIR__ . DIRECTORY_SEPARATOR . 'compare_equality_temp.php';
@unlink($fl);
?>
--EXPECTF--
Failed: 0
===DONE===
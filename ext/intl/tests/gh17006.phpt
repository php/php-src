--TEST--
NumberFormatter::format using new API
--EXTENSIONS--
intl
--FILE--
<?php
$number = 1_000_000;
$oldf = new \NumberFormatter('en_US', \NumberFormatter::SPELLOUT);
$oldv = $oldf->format($number);
var_dump($oldv);
$newf = new \NumberFormatter('en_US', "compact-long");
$newv = $newf->format($number);
var_dump($newv);

$fmt = new \NumberFormatter('en_US', \NumberFormatter::CURRENCY_ACCOUNTING);
var_dump($fmt->formatCurrency($number, 'USD'));

try {
	$fmt->formatCurrency($number);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

$fmt = new \NumberFormatter('en_US', "group-auto currency/USD");
var_dump($fmt->formatCurrency($number));
?>
--EXPECT--
string(11) "one million"
string(9) "1 million"
string(13) "$1,000,000.00"
NumberFormatter::formatCurrency(): Argument #3 currency cannot be null when instantiating NumberFormatterwith a style constant
string(13) "$1,000,000.00"

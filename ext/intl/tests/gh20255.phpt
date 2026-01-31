--TEST--
IntlDateFormatter with PROLEPTIC_GREGORIAN calendar
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(IntlDateFormatter::PROLEPTIC_GREGORIAN);

// A pre-cutover date: DateTime uses proleptic Gregorian internally
$dt = new DateTime('1200-03-01 12:00:00 UTC');

// New constant
$fmt_proleptic = new IntlDateFormatter(
    'en_US', IntlDateFormatter::NONE, IntlDateFormatter::NONE,
    'UTC', IntlDateFormatter::PROLEPTIC_GREGORIAN, 'yyyy-MM-dd'
);

// Existing workaround
$cal = new IntlGregorianCalendar('UTC', 'en_US');
$cal->setGregorianChange(-INF);
$fmt_workaround = new IntlDateFormatter(
    'en_US', IntlDateFormatter::NONE, IntlDateFormatter::NONE,
    'UTC', $cal, 'yyyy-MM-dd'
);

// Default hybrid Gregorian
$fmt_hybrid = new IntlDateFormatter(
    'en_US', IntlDateFormatter::NONE, IntlDateFormatter::NONE,
    'UTC', IntlDateFormatter::GREGORIAN, 'yyyy-MM-dd'
);

$proleptic  = $fmt_proleptic->format($dt);
$workaround = $fmt_workaround->format($dt);
$hybrid     = $fmt_hybrid->format($dt);

// Should round-trip the proleptic Gregorian date correctly
echo "Proleptic: $proleptic\n";

// Must match the manual workaround
echo "Matches workaround: ";
var_dump($proleptic === $workaround);

// Must differ from hybrid for pre-cutover dates
echo "Differs from hybrid: ";
var_dump($proleptic !== $hybrid);

?>
--EXPECT--
int(-1)
Proleptic: 1200-03-01
Matches workaround: bool(true)
Differs from hybrid: bool(true)

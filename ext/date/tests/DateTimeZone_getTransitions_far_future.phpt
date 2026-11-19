--TEST--
DateTimeZone::getTransitions() with a far-future range
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--FILE--
<?php
$tz = new DateTimeZone('Europe/Amsterdam');
var_dump(count($tz->getTransitions(PHP_INT_MAX - 1000, PHP_INT_MAX)));

function dump(string $zone, string $begin, string $end): void {
    echo "$zone $begin - $end\n";
    $tz = new DateTimeZone($zone);
    $b = (new DateTimeImmutable($begin, new DateTimeZone('UTC')))->getTimestamp();
    $e = (new DateTimeImmutable($end, new DateTimeZone('UTC')))->getTimestamp();
    foreach ($tz->getTransitions($b, $e) as $t) {
        echo "{$t['ts']} {$t['offset']} ", (int) $t['isdst'], " {$t['abbr']}\n";
    }
}

dump('Europe/Amsterdam', '2100-01-01', '2110-01-01');
dump('America/New_York', '2101-01-01', '2103-01-01');
dump('Australia/Sydney', '2101-01-01', '2103-01-01');
dump('Australia/Sydney', '2101-01-02', '2101-12-31');
?>
--EXPECT--
int(1)
Europe/Amsterdam 2100-01-01 - 2110-01-01
4102444800 3600 0 CET
4109878800 7200 1 CEST
4128627600 3600 0 CET
4141328400 7200 1 CEST
4160077200 3600 0 CET
4172778000 7200 1 CEST
4191526800 3600 0 CET
4204227600 7200 1 CEST
4222976400 3600 0 CET
4236282000 7200 1 CEST
4254426000 3600 0 CET
4267731600 7200 1 CEST
4285875600 3600 0 CET
4299181200 7200 1 CEST
4317930000 3600 0 CET
4330630800 7200 1 CEST
4349379600 3600 0 CET
4362080400 7200 1 CEST
4380829200 3600 0 CET
4394134800 7200 1 CEST
4412278800 3600 0 CET
America/New_York 2101-01-01 - 2103-01-01
4133980800 -18000 0 EST
4140140400 -14400 1 EDT
4160700000 -18000 0 EST
4171590000 -14400 1 EDT
4192149600 -18000 0 EST
Australia/Sydney 2101-01-01 - 2103-01-01
4133980800 39600 1 AEDT
4141900800 36000 0 AEST
4157625600 39600 1 AEDT
4173350400 36000 0 AEST
4189075200 39600 1 AEDT
Australia/Sydney 2101-01-02 - 2101-12-31
4134067200 39600 1 AEDT
4141900800 36000 0 AEST
4157625600 39600 1 AEDT

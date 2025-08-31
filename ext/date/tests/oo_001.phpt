--TEST--
date OO interface
--FILE--
<?php
date_default_timezone_set('UTC');
class _d extends DateTime {
    function __construct() {
    }
}
class _t extends DateTimeZone {
    function __construct() {
    }
}
class _p extends DatePeriod {
    function __construct() {
    }
}

$d = new DateTime;
var_dump($d->format("Y-m-d H:i:s"));

try {
    $d = new _d;
    var_dump($d->format("Y-m-d H:i:s"));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new DateTime("1am todax");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$t = new DateTimeZone("UTC");
var_dump($t->getName());

try {
    $t = new _t;
    var_dump($t->getName());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new DateTimeZone("GottaFindThisOne");
} catch (DateInvalidTimeZoneException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$p = new _p;
try {
    var_dump($p->getStartDate());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($p->getDateInterval());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "DONE\n";
?>
--EXPECTF--
string(19) "%d-%d-%d %d:%d:%d"
DateObjectError: Object of type _d (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateMalformedStringException: Failed to parse time string (1am todax) at position 4 (t): The timezone could not be found in the database
string(3) "UTC"
DateObjectError: Object of type _t (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (GottaFindThisOne)
DateObjectError: Object of type _p (inheriting DatePeriod) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type _p (inheriting DatePeriod) has not been correctly initialized by calling parent::__construct() in its constructor
DONE

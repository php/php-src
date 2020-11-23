--TEST--
Bug #34676 (missing support for strtotime("midnight") and strtotime("noon"))
--FILE--
<?php
date_default_timezone_set("UTC");

$tests = array(
    'noon', 'midnight'
);

foreach ($tests as $test) {
    $t = strtotime("2005-12-22 ". $test);
    printf("%-10s => %s\n", $test, date(DATE_ISO8601, $t));
}

?>
--EXPECT--
noon       => 2005-12-22T12:00:00+0000
midnight   => 2005-12-22T00:00:00+0000

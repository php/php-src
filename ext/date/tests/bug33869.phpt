--TEST--
Bug #33869 (strtotime() doesn't parse "+1days" format)
--FILE--
<?php
    date_default_timezone_set("UTC");
    $tm = strtotime("2005-01-01 01:01:01");
    echo date(DATE_ISO8601, strtotime('+5days', $tm));
    echo "\n";
    echo date(DATE_ISO8601, strtotime('+1month', $tm));
    echo "\n";
    echo date(DATE_ISO8601, strtotime('+1year', $tm));
    echo "\n";
    echo date(DATE_ISO8601, strtotime('+5 days', $tm));
    echo "\n";
    echo date(DATE_ISO8601, strtotime('+1 month', $tm));
    echo "\n";
    echo date(DATE_ISO8601, strtotime('+1 year', $tm));
    echo "\n";
?>
--EXPECT--
2005-01-06T01:01:01+0000
2005-02-01T01:01:01+0000
2006-01-01T01:01:01+0000
2005-01-06T01:01:01+0000
2005-02-01T01:01:01+0000
2006-01-01T01:01:01+0000

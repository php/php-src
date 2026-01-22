--TEST--
Bug GH-13857 (Unexpected Results with date_diff for Different Timezones) wall-clock dates are inverted.
--FILE--
<?php
date_default_timezone_set('Asia/Tokyo');
$nowDate = (new DateTime('2024-04-01 00:49:22'))->setTimeZone(new DateTimeZone('Asia/Tokyo'));
$endDate = (new DateTime('2024-04-01 08:48:22'))->setTimeZone(new DateTimeZone('UTC'));

echo "nowDate: ", $nowDate->format('Y-m-d H:i:s T'), "\n";
echo "endDate: ", $endDate->format('Y-m-d H:i:s T'), "\n";

$diff = date_diff($nowDate, $endDate);
echo "diff: ", $diff->format('%R %Y-%M-%D %H:%I:%S'), "\n";
echo "days: ", $diff->days, "\n";

$diff2 = date_diff($endDate, $nowDate);
echo "reversed: ", $diff2->format('%R %Y-%M-%D %H:%I:%S'), "\n";

$a = new DateTime('2024-04-01 00:49:22', new DateTimeZone('+09:00'));
$b = new DateTime('2024-03-31 23:48:22', new DateTimeZone('+00:00'));
$diff3 = $a->diff($b);
echo "offset TZ: ", $diff3->format('%R %Y-%M-%D %H:%I:%S'), "\n";
?>
--EXPECT--
nowDate: 2024-04-01 00:49:22 JST
endDate: 2024-03-31 23:48:22 UTC
diff: + 00-00-00 07:59:00
days: 0
reversed: - 00-00-00 07:59:00
offset TZ: + 00-00-00 07:59:00

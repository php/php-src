--TEST--
Bug ##74013 (DateTime not able to handle DateInterval on february)
--FILE--
<?php
if ((new DateTime('2017-01-31'))->add(new DateInterval('P1M')) >
    (new DateTime('2017-02-01'))->add(new DateInterval('P1M'))) {
    echo "Failed\n";
} else {
    echo "Success\n";
}


echo (new DateTime("2017-01-28"))->add(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2017-01-29"))->add(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2017-01-30"))->add(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2017-01-31"))->add(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2017-02-01"))->add(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2016-02-29"))->add(new DateInterval("P1Y"))->format('Y-m-d') . "\n";

echo (new DateTime("2017-01-30"))->add(new DateInterval("P1M1D"))->format('Y-m-d') . "\n";

echo (new DateTime("2017-03-30"))->add(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2017-03-31"))->add(new DateInterval("P1M"))->format('Y-m-d') . "\n";

echo (new DateTime("2017-03-28"))->sub(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2017-03-29"))->sub(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2017-03-30"))->sub(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2017-03-31"))->sub(new DateInterval("P1M"))->format('Y-m-d') . "\n";
echo (new DateTime("2017-02-28"))->sub(new DateInterval("P1M"))->format('Y-m-d') . "\n";
?>
--EXPECTF--
Success
2017-02-28
2017-02-28
2017-02-28
2017-02-28
2017-03-01
2017-02-28
2017-03-01
2017-04-30
2017-04-30
2017-02-28
2017-02-28
2017-02-28
2017-02-28
2017-01-28

--TEST--
Bug #79403 (DateTime::construct inconsistent results with "first day of")
--FILE--
<?
echo (new DateTime("first day of this month"))->format('H:i:s') . "\n";
echo (new DateTime("last day of this month"))->format('H:i:s') . "\n";
echo (new DateTime("first day of next month"))->format('H:i:s') . "\n";
echo (new DateTime("last day of next month"))->format('H:i:s') . "\n";
?>
--EXPECT--
00:00:00
00:00:00
00:00:00
00:00:00

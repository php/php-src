--TEST--
microtime() function
--POST--
--GET--
--FILE--
<?php
$passed = 0;
$failed = 0;
$last = 0;

set_time_limit(0);

for ($i=1;$i<=100000;$i++) {
   list($micro,$time)=explode(" ",microtime());
   $add=$micro+$time;
   $add<$last ? $failed++: $passed++;
   $last=$add;
}
echo "Passed: ".$passed."\n";
echo "Failed: ".$failed."\n";
?>
--EXPECT--
Passed: 100000
Failed: 0
--TEST--
microtime() function
--SKIPIF--
<?php
	if (!function_exists('microtime'))  die('skip microtime() not available');
?>
--FILE--
<?php
$passed = 0;
$failed = 0;
$last_m = 0;
$last_t = 0;
$result = '';

set_time_limit(0);

for ($i=1;$i<=100000;$i++) {
	list($micro,$time)=explode(" ",microtime());
	if ($time > $last_t || ($time == $last_t && $micro >= $last_m)) {
		$passed++;
	} else if ($failed++ <=10) {
		$result .= sprintf('%06d', $i).": $time $micro < $last_t $last_m\n";
	}
	$last_m = $micro;
	$last_t = $time;
}
echo "Passed: $passed\n";
echo "Failed: $failed\n";
echo $result;
?>
--EXPECT--
Passed: 100000
Failed: 0

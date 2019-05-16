--TEST--
Bug #30096 (gmmktime does not return the corrent time)
--INI--
error_reporting=2047
--FILE--
<?php
echo "no dst --> dst\n";
$ts = -1;
gm_date_check(01,00,00,03,27,2005);
gm_date_check(02,00,00,03,27,2005);
gm_date_check(03,00,00,03,27,2005);
gm_date_check(04,00,00,03,27,2005);

echo "\ndst --> no dst\n";
$ts = -1;
gm_date_check(01,00,00,10,30,2005);
gm_date_check(02,00,00,10,30,2005);
gm_date_check(03,00,00,10,30,2005);
gm_date_check(04,00,00,10,30,2005);

function gm_date_check($hour, $minute, $second, $month, $day, $year) {
	global $ts, $tsold;

	echo "gmmktime($hour,$minute,$second,$month,$day,$year): ";

	$tsold = $ts;
	$ts = gmmktime($hour, $minute, $second, $month, $day, $year);

	echo $ts, " | gmdate('r', $ts):", gmdate('r', $ts);
	if ($tsold > 0) {
		echo " | Diff: " . ($ts - $tsold);
	}
	echo "\n";
}

?>
--EXPECT--
no dst --> dst
gmmktime(1,0,0,3,27,2005): 1111885200 | gmdate('r', 1111885200):Sun, 27 Mar 2005 01:00:00 +0000
gmmktime(2,0,0,3,27,2005): 1111888800 | gmdate('r', 1111888800):Sun, 27 Mar 2005 02:00:00 +0000 | Diff: 3600
gmmktime(3,0,0,3,27,2005): 1111892400 | gmdate('r', 1111892400):Sun, 27 Mar 2005 03:00:00 +0000 | Diff: 3600
gmmktime(4,0,0,3,27,2005): 1111896000 | gmdate('r', 1111896000):Sun, 27 Mar 2005 04:00:00 +0000 | Diff: 3600

dst --> no dst
gmmktime(1,0,0,10,30,2005): 1130634000 | gmdate('r', 1130634000):Sun, 30 Oct 2005 01:00:00 +0000
gmmktime(2,0,0,10,30,2005): 1130637600 | gmdate('r', 1130637600):Sun, 30 Oct 2005 02:00:00 +0000 | Diff: 3600
gmmktime(3,0,0,10,30,2005): 1130641200 | gmdate('r', 1130641200):Sun, 30 Oct 2005 03:00:00 +0000 | Diff: 3600
gmmktime(4,0,0,10,30,2005): 1130644800 | gmdate('r', 1130644800):Sun, 30 Oct 2005 04:00:00 +0000 | Diff: 3600

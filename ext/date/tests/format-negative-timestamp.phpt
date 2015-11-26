--TEST--
strtotime() - Format: @timestamps
--FILE--
<?php
date_default_timezone_set("GMT");

$i = 5;
$max = getrandmax();
$max_2 = $max / 2;

while($i--) {
	$new_tm = rand(1, $max);
	if ($new_tm > $max_2)
		$new_tm *= -1;

	if (strtotime("@$new_tm") != $new_tm) {
		echo "Error when parsing: @$new_tm\n";
	}
}

echo "done!";
?>
--EXPECT--
done!

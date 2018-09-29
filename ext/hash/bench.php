<?php

/*
 This gives rather interesting results :)

 Measures on a Notebook P4M-1.7 256MB Windows 2000:
	sha1         0.556691
	tiger160,3   0.774469
	tiger192,3   0.776314
	tiger128,3   0.777004
	ripemd128    0.896674
	sha256       1.011164
	md5          1.016032
	tiger160,4   1.056617
	tiger128,4   1.063101
	tiger192,4   1.069258
	haval160,3   1.125099
	haval128,3   1.125679
	haval224,3   1.128017
	haval192,3   1.130026
	haval256,3   1.134846
	ripemd160    1.150693
	haval128,4   1.686261
	haval192,4   1.687274
	haval160,4   1.693091
	haval256,4   1.699323
	haval224,4   1.743094
	haval160,5   2.003452
	haval192,5   2.008341
	haval256,5   2.009048
	haval128,5   2.009555
	haval224,5   2.015539
	sha384       3.370734
	sha512       3.381121
	whirlpool    6.912327
	snefru       9.268168

 Measures on a Desktop P4-2.4 512MB Debian (Linux-2.4):
	md5          0.147739
	haval128,3   0.317006
	haval192,3   0.317524
	haval256,3   0.317526
	haval160,3   0.323035
	haval224,3   0.333318
	ripemd128    0.353447
	sha1         0.376200
	ripemd160    0.413758
	sha256       0.435957
	haval160,4   0.452357
	haval224,4   0.454531
	haval128,4   0.458026
	haval256,4   0.459051
	haval192,4   0.468094
	haval128,5   0.524262
	haval160,5   0.529573
	haval224,5   0.533655
	haval256,5   0.534446
	haval192,5   0.543726
	tiger128,3   0.577975
	tiger160,3   0.579951
	tiger192,3   0.597111
	tiger192,4   0.781408
	tiger160,4   0.801243
	tiger128,4   0.812239
	sha512       1.298627
	sha384       1.313607
	whirlpool    1.556159
	snefru       5.703742

*/

error_reporting(E_ALL&~E_NOTICE);

$data = file_get_contents(__FILE__);
$time = array();

for ($j = 0; $j < 10; $j++) {
	foreach (hash_algos() as $algo) {
		$start = microtime(true);
		for ($i = 0; $i < 1000; $i++) {
			hash($algo, $data);
		}
		$time[$algo] += microtime(true)-$start;
	}
}

asort($time, SORT_NUMERIC);
foreach ($time as $a => $t) {
	printf("%-12s %02.6f\n", $a, $t);
}
?>

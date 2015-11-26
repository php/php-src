--TEST--
crypt() SHA-256 
--SKIPIF--
<?php
if (!function_exists('crypt') || !defined("CRYPT_SHA256")) {
	die("SKIP crypt()-sha256 is not available");
}
?> 
--FILE--
<?php

$tests = array(
	1 => array(
		b'$5$saltstring',
		b'Hello world!',
		b'$5$saltstring$5B8vYYiY.CVt1RlTTf8KbXBH3hsxY/GNooZaBBGWEc5'
	),
	2 => array(
		b'$5$rounds=10000$saltstringsaltstring',
		b'Hello world!',
		b'$5$rounds=10000$saltstringsaltst$3xv.VbSHBb41AL9AvLeujZkZRBAwqFMz2.opqey6IcA'
	),
	3 => array(
		b'$5$rounds=10000$saltstringsaltstring',
		b'Hello world!',
		b'$5$rounds=10000$saltstringsaltst$3xv.VbSHBb41AL9AvLeujZkZRBAwqFMz2.opqey6IcA'
	),
	4 => array(
		b'$5$rounds=5000$toolongsaltstring',
		b'This is just a test',
		b'$5$rounds=5000$toolongsaltstrin$Un/5jzAHMgOGZ5.mWJpuVolil07guHPvOW8mGRcvxa5'
	),
	5 => array(
		b'$5$rounds=1400$anotherlongsaltstring',
		b'a very much longer text to encrypt.  This one even stretches over morethan one line.',
		b'$5$rounds=1400$anotherlongsalts$Rx.j8H.h8HjEDGomFU8bDkXm3XIUnzyxf12oP84Bnq1'
	),
	6 => array(
		b'$5$rounds=77777$short',
		b'we have a short salt string but not a short password',
		b'$5$rounds=77777$short$JiO1O3ZpDAxGJeaDIuqCoEFysAe1mZNJRs3pw0KQRd/'
	),
	7 => array(
		b'$5$rounds=123456$asaltof16chars..',
		b'a short string',
		b'$5$rounds=123456$asaltof16chars..$gP3VQ/6X7UUEW3HkBn2w1/Ptq2jxPyzV/cZKmF/wJvD'
	),
	8 => array(
		b'$5$rounds=10$roundstoolow',
		b'the minimum number is still observed',
		b'$5$rounds=1000$roundstoolow$yfvwcWrQ8l/K0DAWyuPMDNHpIVlTQebY9l/gL972bIC'
	)
);

foreach ($tests as $iter => $t) {
	$res = crypt($t[1], $t[0]);
	if ($res != $t[2]) echo "Iteration $iter failed.
Expected: <$t[2]>
Got       <$res>\n";
}
echo "Passes.";?>
--EXPECTF--
Passes.


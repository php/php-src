--TEST--
mb_strcut() re-designates JIS X 0213 plane when switching between planes 1 and 2 in ISO-2022-JP-2004
--EXTENSIONS--
mbstring
--FILE--
<?php
foreach (["\u{4FF1}\u{20089}", "\u{20089}\u{4FF1}", "\u{20089}\u{301}"] as $u) {
	$s = mb_convert_encoding($u, 'ISO-2022-JP-2004', 'UTF-8');
	$cut = mb_strcut($s, 0, strlen($s), 'ISO-2022-JP-2004');
	echo bin2hex($cut), ' ', var_export($cut === $s, true), ' ', bin2hex(mb_convert_encoding($cut, 'UTF-8', 'ISO-2022-JP-2004')), "\n";
}
?>
--EXPECT--
1b2428512e211b24285021211b2842 true e4bfb1f0a08289
1b24285021211b2428512e211b2842 true f0a08289e4bfb1
1b24285021211b2428512b5a1b2842 true f0a08289cc81

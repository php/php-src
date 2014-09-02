--TEST--
Bug #50175: gmp_init() results 0 on given base and number starting with 0x or 0b
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php
for ($base = 2; $base <= 36; $base++) {
	echo "$base:";
	for ($i = 1; $i < $base; $i++) {
		$c = $i <= 9 ? $i : chr($i - 10 + ord('a'));
		echo ' ' . gmp_strval(gmp_init("0$c$c", $base), $base);
	}
	echo "\n";
}
?>
--EXPECT--
2: 11
3: 11 22
4: 11 22 33
5: 11 22 33 44
6: 11 22 33 44 55
7: 11 22 33 44 55 66
8: 11 22 33 44 55 66 77
9: 11 22 33 44 55 66 77 88
10: 11 22 33 44 55 66 77 88 99
11: 11 22 33 44 55 66 77 88 99 aa
12: 11 22 33 44 55 66 77 88 99 aa bb
13: 11 22 33 44 55 66 77 88 99 aa bb cc
14: 11 22 33 44 55 66 77 88 99 aa bb cc dd
15: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee
16: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff
17: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg
18: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh
19: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii
20: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj
21: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk
22: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll
23: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm
24: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn
25: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo
26: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp
27: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq
28: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr
29: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss
30: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt
31: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu
32: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu vv
33: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu vv ww
34: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu vv ww xx
35: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu vv ww xx yy
36: 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu vv ww xx yy zz

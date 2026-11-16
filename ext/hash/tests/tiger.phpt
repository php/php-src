--TEST--
Hash: Tiger algorithm
--FILE--
<?php
foreach ([3, 4] as $pass) {
	foreach ([128, 160] as $bits) {
		$algo = "tiger{$bits},{$pass}";
		echo "{$algo} is substring of tiger192,{$pass}: ", var_export(hash($algo, $algo) === substr(hash("tiger192,{$pass}", $algo), 0, ($bits / 8) * 2), true), "\n";
	}
}

// https://www.cs.technion.ac.il/~biham/Reports/Tiger/
// https://biham.cs.technion.ac.il/Reports/Tiger/testresults.html
// There are no authoritative tests for the 4-pass version
echo "*** Tiger reference (3-pass only) ***\n";
echo hash("tiger192,3", ""), "\n";
echo hash("tiger192,3", "abc"), "\n";
echo hash("tiger192,3", "Tiger"), "\n";
echo hash("tiger192,3", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-"), "\n";
echo hash("tiger192,3", "ABCDEFGHIJKLMNOPQRSTUVWXYZ=abcdefghijklmnopqrstuvwxyz+0123456789"), "\n";
echo hash("tiger192,3", "Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham"), "\n";
echo hash("tiger192,3", "Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham, proceedings of Fast Software Encryption 3, Cambridge."), "\n";
echo hash("tiger192,3", "Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham, proceedings of Fast Software Encryption 3, Cambridge, 1996."), "\n";
echo hash("tiger192,3", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-"), "\n";
echo hash("tiger192,3", str_repeat(pack("C*", ... range(0, 255)), 256)), "\n";
?>
--EXPECT--
tiger128,3 is substring of tiger192,3: true
tiger160,3 is substring of tiger192,3: true
tiger128,4 is substring of tiger192,4: true
tiger160,4 is substring of tiger192,4: true
*** Tiger reference (3-pass only) ***
3293ac630c13f0245f92bbb1766e16167a4e58492dde73f3
2aab1484e8c158f2bfb8c5ff41b57a525129131c957b5f93
dd00230799f5009fec6debc838bb6a27df2b9d6f110c7937
f71c8583902afb879edfe610f82c0d4786a3a534504486b5
48ceeb6308b87d46e95d656112cdf18d97915f9765658957
8a866829040a410c729ad23f5ada711603b3cdd357e4c15e
ce55a6afd591f5ebac547ff84f89227f9331dab0b611c889
631abdd103eb9a3d245b6dfd4d77b257fc7439501d1568dd
c54034e5b43eb8005848a7e0ae6aac76e4ff590ae715fd25
fdf4f5b35139f48e710e421be5af411de1a8aac333f26204

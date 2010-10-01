--TEST--
Feature Request #50283 (allow base in gmp_strval to use full range: 2 to 62, and -2 to -36)
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
<?php if (version_compare(GMP_VERSION, "4.2.0", "<")) print "skip"; ?>
--FILE--
<?php
$a = gmp_init("0x41682179fbf5");
printf("Decimal: %s, -36-based: %s\n", gmp_strval($a), gmp_strval($a,-36));
printf("Decimal: %s, 36-based: %s\n", gmp_strval($a), gmp_strval($a,36));
printf("Decimal: %s, -1-based: %s\n", gmp_strval($a), gmp_strval($a,-1));
printf("Decimal: %s, 1-based: %s\n", gmp_strval($a), gmp_strval($a,1));
printf("Decimal: %s, -37-based: %s\n", gmp_strval($a), gmp_strval($a,-37));
printf("Decimal: %s, 37-based: %s\n", gmp_strval($a), gmp_strval($a,37));
printf("Decimal: %s, 62-based: %s\n", gmp_strval($a), gmp_strval($a,62));
printf("Decimal: %s, 63-based: %s\n\n", gmp_strval($a), gmp_strval($a,63));
printf("Base 32 and 62-based: %s\n", gmp_strval(gmp_init("gh82179fbf5", 32), 62));
?>
--EXPECTF--
Decimal: 71915494046709, -36-based: PHPISCOOL
Decimal: 71915494046709, 36-based: phpiscool

Warning: gmp_strval(): Bad base for conversion: -1 (should be between 2 and %d or -2 and -%d) in %s on line 5
Decimal: 71915494046709, -1-based: 

Warning: gmp_strval(): Bad base for conversion: 1 (should be between 2 and %d or -2 and -%d) in %s on line 6
Decimal: 71915494046709, 1-based: 

Warning: gmp_strval(): Bad base for conversion: -37 (should be between 2 and %d or -2 and -%d) in %s on line 7
Decimal: 71915494046709, -37-based: 
Decimal: 71915494046709, 37-based: KHKATELJF
Decimal: 71915494046709, 62-based: KQ6yq741

Warning: gmp_strval(): Bad base for conversion: 63 (should be between 2 and %d or -2 and -%d) in %s on line 10
Decimal: 71915494046709, 63-based: 

Base 32 and 62-based: 1NHkAcdIiD

--TEST--
Feature Request #50283 (allow base in gmp_strval to use full range: 2 to 62, and -2 to -36)
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php
$a = gmp_init("0x41682179fbf5");
printf("Decimal: %s, 36-based: %s\n", gmp_strval($a), gmp_strval($a,-36));
printf("Decimal: %s, 36-based: %s\n", gmp_strval($a), gmp_strval($a,36));
printf("Decimal: %s, 36-based: %s\n", gmp_strval($a), gmp_strval($a,-1));
printf("Decimal: %s, 36-based: %s\n", gmp_strval($a), gmp_strval($a,1));
printf("Decimal: %s, 36-based: %s\n", gmp_strval($a), gmp_strval($a,-37));
printf("Decimal: %s, 36-based: %s\n", gmp_strval($a), gmp_strval($a,37));
?>
--EXPECTF--
Decimal: 71915494046709, 36-based: PHPISCOOL
Decimal: 71915494046709, 36-based: phpiscool

Warning: gmp_strval(): Bad base for conversion: -1 in %s on line 5
Decimal: 71915494046709, 36-based: 

Warning: gmp_strval(): Bad base for conversion: 1 in %s on line 6
Decimal: 71915494046709, 36-based: 

Warning: gmp_strval(): Bad base for conversion: -37 in %s on line 7
Decimal: 71915494046709, 36-based: 

Warning: gmp_strval(): Bad base for conversion: 37 in %s on line 8
Decimal: 71915494046709, 36-based: 

--TEST--
PEAR_Common::validPackageVersion test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php

require_once 'PEAR/Common.php';

// '\d+(?:\.\d+)*(?:[a-z]+\d*)?'

echo "==Valid Tests==\n";
$a = '1';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1.1';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1.1.1';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1.1.1.1';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1.1.1abc3';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1.234beta4';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1alpha3';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1alpha';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1.1a';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

echo "==Invalid Tests==\n";

$a = '1.0.0-alpha2';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1alpha.4';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";

$a = '1.1alpha.4';
echo "$a ";
echo (PEAR_Common::validPackageVersion($a)) ? "valid\n" : "invalid\n";
?>
--GET--
--POST--
--EXPECT--
==Valid Tests==
1 valid
1.1 valid
1.1.1 valid
1.1.1.1 valid
1.1.1abc3 valid
1.234beta4 valid
1alpha3 valid
1alpha valid
1.1a valid
==Invalid Tests==
1.0.0-alpha2 invalid
1alpha.4 invalid
1.1alpha.4 invalid

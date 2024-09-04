--TEST--
BcMath\Number operator short syntax
--EXTENSIONS--
bcmath
--FILE--
<?php

echo "========== add ==========\n";
$num = new BcMath\Number(100);
$num_old = $num;
$num += 100;
echo $num . "\n";
$num += '50';
echo $num . "\n";
$num += (new BcMath\Number(30));
echo $num . "\n";
echo "\n";

echo "========== sub ==========\n";
$num = new BcMath\Number(100);
$num_old = $num;
$num -= 100;
echo $num . "\n";
$num -= '50';
echo $num . "\n";
$num -= (new BcMath\Number(30));
echo $num . "\n";
echo "\n";

echo "========== mul ==========\n";
$num = new BcMath\Number(100);
$num_old = $num;
$num *= 100;
echo $num . "\n";
$num *= '50';
echo $num . "\n";
$num *= (new BcMath\Number(30));
echo $num . "\n";
echo "\n";

echo "========== div ==========\n";
$num = new BcMath\Number(100);
$num_old = $num;
$num /= 100;
echo $num . "\n";
$num /= '50';
echo $num . "\n";
$num /= (new BcMath\Number(30));
echo $num . "\n";
echo "\n";

echo "========== mod ==========\n";
$num = new BcMath\Number(1000);
$num_old = $num;
$num %= 90;
echo $num . "\n";
$num %= '30';
echo $num . "\n";
$num %= (new BcMath\Number(6));
echo $num . "\n";
echo "\n";

echo "========== pow ==========\n";
$num = new BcMath\Number(10);
$num_old = $num;
$num **= 2;
echo $num . "\n";
$num **= '3';
echo $num . "\n";
$num **= (new BcMath\Number(0));
echo $num . "\n";
echo "\n";
?>
--EXPECT--
========== add ==========
200
250
280

========== sub ==========
0
-50
-80

========== mul ==========
10000
500000
15000000

========== div ==========
1
0.02
0.000666666666

========== mod ==========
10
10
4

========== pow ==========
100
1000000
1

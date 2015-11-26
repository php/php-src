--TEST--
Execution order of variables
--FILE--
<?php

/* strings and concat */

class strclass {
   var $str = "bad";
   static $statstr = "bad";
}

$a = "bad";
$b = "good";
echo "1)";
$c = $a.($a=$b);
echo $c;
echo "\r\n";

$a = "bad";
$b = "good";
$c = ($a=$b).$a;
echo "2)";
echo $c;
echo "\r\n";


$str = new strclass();
$c = $str->str.($str->str="good");
echo "3)";
echo $c;
echo "\r\n";

$str->str = "bad";

$c = ($str->str="good").$str->str;
echo "4)";
echo $c;
echo "\r\n";

$c = strclass::$statstr.(strclass::$statstr="good");
echo "5)";
echo $c;
echo "\r\n";

strclass::$statstr = "bad";

$c = (strclass::$statstr="good").strclass::$statstr;
echo "6)";
echo $c;
echo "\r\n";


function foo() {
   global $a;
   $a = "good";
   return $a;
}


$a = "bad";
echo "7)";
echo foo() . $a;
echo "\r\n";

$a = "bad";
echo "8)";
echo $a . foo();
echo "\r\n";

/* other operators */

$x = 1;
$z = $x - ($x++);
echo "9)";
echo $z;
echo "\r\n";

$x = 1;
$z = ($x++) - $x;
echo "10)";
echo $z;
echo "\r\n";

$x = 1;
$z = $x - (++$x);
echo "11)";
echo $z;
echo "\r\n";

$x = 1;
$z = (++$x) - $x;
echo "12)";
echo $z;
echo "\r\n";


$x = 1;
$y = 3;
$z = $x - ($x=$y);
echo "13)";
echo $z;
echo "\r\n";

$x = 1;
$y = 3;
$z = ($x=$y) - $x;
echo "14)";
echo $z;
echo "\r\n";


$a = 100;
$b = 200;
echo "15)";
echo $a + ($a=$b);
echo "\r\n";

$a = 100;
$b = 200;
echo "16)";
echo ($a=$b) + $a;
echo "\r\n";


$a = array(100,200);
$i = 0;

echo "17)";
echo $a[$i++] + $a[$i++];
echo "\r\n";

$i = -1;
echo "18)";
echo $a[++$i] + $a[++$i];
echo "\r\n";

$i = 0;
echo "19)";
echo $a[$i] + ($a[$i]=400);
echo "\r\n";


$a[0] = 100;
echo "20)";
echo ($a[$i]=400) + $a[$i];
echo "\r\n";


class c {
   var $val = 10;
   static $stat = 20;
}

echo "21)";
echo c::$stat + (c::$stat=200);
echo "\r\n";

echo "22)";
echo (c::$stat=300) + c::$stat;
echo "\r\n";

$c = new c();

echo "23)";
echo $c->val + ($c->val=200);
echo "\r\n";

echo "24)";
echo ($c->val=300) + $c->val;
echo "\r\n";
 
?>
--EXPECT--
1)goodgood
2)goodgood
3)badgood
4)goodgood
5)badgood
6)goodgood
7)goodgood
8)goodgood
9)1
10)-1
11)0
12)0
13)0
14)0
15)400
16)400
17)300
18)300
19)500
20)800
21)220
22)600
23)210
24)600

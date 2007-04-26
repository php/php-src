--TEST--
Test printf() function (32bit)
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_MAX > 2147483647) {
        die("skip 32bit test only");
}
?>
--FILE--
<?php

/* Various input arrays for different format types */

$float_variation  = array( "%f", "%-f", "%+f", "%7.2f", "%-7.2f", "%07.2f", "%-07.2f", "%'#7.2f" );
$float_numbers    = array( 0, 1, -1, 0.32, -0.32, 3.4. -3.4, 2.54, -2.54, 1.2345678e99, -1.2345678e99 );

$int_variation    = array( "%d", "%-d", "%+d", "%7.2d", "%-7.2d", "%07.2d", "%-07.2d", "%'#7.2d" );
$int_numbers      = array( 0, 1, -1, 2.7, -2.7, 23333333, -23333333, "1234" );

$char_variation   = array( 'a', "a", 67, -67, 99 );

$string_variation = array( "%5s", "%-5s", "%05s", "%'#5s" );
$strings          = array( NULL, "abc", 'aaa' );

/* Checking warning messages */

/* Zero argument */
echo "\n*** Output for zero argument ***\n";
printf();

/* Number of arguments not matching as specified in format field */
echo "\n*** Output for insufficient number of arguments ***\n";
$string = "dingy%sflem%dwombat";
$nbr = 5;
$name = "voudras";
printf("%d $string %s", $nbr, $name);


/* Scalar argument */
echo "\n*** Output for scalar argument ***\n";
printf(3);

/* NULL argument */
echo "\n*** Output for NULL as argument ***\n";
printf(NULL);


/* Float type variations */

$counter = 1;
echo "\n\n*** Output for float type ***\n";
echo "\n Input Float numbers variation array is:\n";
print_r($float_numbers);

foreach( $float_variation as $float_var )
{
 echo "\n\nFloat Iteration $counter";
 foreach( $float_numbers as $float_num )
 {
  echo "\n";
  printf( $float_var, $float_num );
 }
 $counter++;
}


/* Integer type variations */

$counter = 1;
echo "\n\n*** Output for integer type ***\n";
echo "\n Input Integer numbers variation array is:\n";
print_r($int_numbers);

foreach( $int_variation as $int_var )
{
 echo "\n\nInteger Iteration $counter";
 foreach( $int_numbers as $int_num )
 {
  echo "\n";
  printf( $int_var, $int_num );
 }
 $counter++;
}


/* Binary type variations */

echo "\n\n*** Output for binary type ***\n";
echo "\n Input  numbers variation array is:\n";
print_r($int_numbers);

 foreach( $int_numbers as $bin_num )
 {
  echo "\n";
  printf( "%b", $bin_num );
 }


/* Chararter type variations */
echo "\n\n*** Output for char type ***\n";
echo "\n Input Characters variation array is:\n";
print_r($char_variation);

foreach( $char_variation as $char )
{
 echo "\n";
 printf( "%c", $char );
}

/* Scientific type variations */
echo "\n\n*** Output for scientific type ***\n";
echo "\n Input numbers variation array is:\n";
print_r($int_numbers);

foreach( $int_numbers as $num )
{
 echo "\n";
 printf( "%e", $num );
}

/* Unsigned Integer type variation */
echo "\n\n*** Output for unsigned integer type ***\n";
echo "\n Input Integer numbers variation array is:\n";
print_r($int_numbers);

foreach( $int_numbers as $unsig_num )
{
 echo "\n";
 printf( "%u", $unsig_num );
}

/* Octal type variations */
echo "\n\n*** Output for octal type ***\n";
echo "\n Input numbers variation array is:\n";
print_r($int_numbers);

foreach( $int_numbers as $octal_num )
{
 echo "\n";
 printf( "%o", $octal_num );
}

/* Hexadecimal type variations */
echo "\n\n*** Output for hexadecimal type ***\n";
echo "\n Input numbers variation array is:\n";
print_r($int_numbers);

foreach( $int_numbers as $hexa_num )
{
 echo "\n";
 printf( "%x", $hexa_num );
}

/* String type variations */
echo "\n\n*** Output for string type ***\n";
echo "\n Input Strings format variation array is:\n";
print_r($string_variation);
echo "\n Input strings variation array is:\n";
print_r($strings);

foreach( $string_variation as $string_var )
{
 foreach( $strings as $str )
 {
  echo "\n";
  printf( $string_var, $str );
 }
}


/* variations of %g type */
$format_g = array("%g", "%.0g", "%+g", "%-g", "%-1.2g", "%+1.2g", "%G", "%.0G", "%+G", "%-G", "%-1.2G", "%+1.2G");

echo "\n\n*** Output for '%g' type ***\n";
echo "\n Input format variation array is:\n";
print_r($format_g);

foreach( $format_g as $formatg )
{
 printf("\n$formatg",123456);
 printf("\n$formatg",-123456);
}


/* Some more typical cases */

$tempnum = 12345;
$tempstring = "abcdefghjklmnpqrstuvwxyz";

echo"\n\n*** Output for '%%%.2f' as the format parameter ***\n";
printf("%%%.2f",1.23456789e10);

echo"\n\n*** Output for '%%' as the format parameter ***\n";
printf("%%",1.23456789e10);

echo"\n\n*** Output for precision value more than maximum ***\n";
printf("%.988f",1.23456789e10);   

echo"\n\n*** Output for invalid width(-15) specifier ***\n";
printf("%030.-15s", $tempstring);  

echo"\n\n*** Output for '%F' as the format parameter ***\n";
printf("%F",1.23456789e10);

echo"\n\n*** Output for '%X' as the format parameter ***\n";
printf("%X",12);

echo"\n\n*** Output  with no format parameter ***\n";
printf($tempnum);

echo"\n\n*** Output for multiple format parameters ***\n";
printf("%d  %s  %d\n", $tempnum, $tempstring, $tempnum); 

echo"\n\n*** Output for excess of mixed type arguments  ***\n";
printf("%s", $tempstring, $tempstring, $tempstring);      

echo"\n\n*** Output for string format parameter and integer type argument ***\n";
printf("%s", $tempnum);          

echo"\n\n*** Output for integer format parameter and string type argument ***\n";
printf("%d", $tempstring);    


?>
--EXPECTF--
*** Output for zero argument ***

Warning: Wrong parameter count for printf() in %s on line %d

*** Output for insufficient number of arguments ***

Warning: printf(): Too few arguments in %s on line %d

*** Output for scalar argument ***
3
*** Output for NULL as argument ***


*** Output for float type ***

 Input Float numbers variation array is:
Array
(
    [0] => 0
    [1] => 1
    [2] => -1
    [3] => 0.32
    [4] => -0.32
    [5] => 3.4-3.4
    [6] => 2.54
    [7] => -2.54
    [8] => 1.2345678E+99
    [9] => -1.2345678E+99
)


Float Iteration 1
0.000000
1.000000
-1.000000
0.320000
-0.320000
3.400000
2.540000
-2.540000
1234567%d.000000
-1234567%d.000000

Float Iteration 2
0.000000
1.000000
-1.000000
0.320000
-0.320000
3.400000
2.540000
-2.540000
1234567%d.000000
-1234567%d.000000

Float Iteration 3
+0.000000
+1.000000
-1.000000
+0.320000
-0.320000
+3.400000
+2.540000
-2.540000
+1234567%d.000000
-1234567%d.000000

Float Iteration 4
   0.00
   1.00
  -1.00
   0.32
  -0.32
   3.40
   2.54
  -2.54
1234567%d.00
-1234567%d.00

Float Iteration 5
0.00   
1.00   
-1.00  
0.32   
-0.32  
3.40   
2.54   
-2.54  
1234567%d.00
-1234567%d.00

Float Iteration 6
0000.00
0001.00
-001.00
0000.32
-000.32
0003.40
0002.54
-002.54
1234567%d.00
-1234567%d.00

Float Iteration 7
0.00000
1.00000
-1.0000
0.32000
-0.3200
3.40000
2.54000
-2.5400
1234567%d.00
-1234567%d.00

Float Iteration 8
###0.00
###1.00
##-1.00
###0.32
##-0.32
###3.40
###2.54
##-2.54
1234567%d.00
-1234567%d.00

*** Output for integer type ***

 Input Integer numbers variation array is:
Array
(
    [0] => 0
    [1] => 1
    [2] => -1
    [3] => 2.7
    [4] => -2.7
    [5] => 23333333
    [6] => -23333333
    [7] => 1234
)


Integer Iteration 1
0
1
-1
2
-2
23333333
-23333333
1234

Integer Iteration 2
0
1
-1
2
-2
23333333
-23333333
1234

Integer Iteration 3
+0
+1
-1
+2
-2
+23333333
-23333333
+1234

Integer Iteration 4
      0
      1
     -1
      2
     -2
23333333
-23333333
   1234

Integer Iteration 5
0      
1      
-1     
2      
-2     
23333333
-23333333
1234   

Integer Iteration 6
0000000
0000001
-000001
0000002
-000002
23333333
-23333333
0001234

Integer Iteration 7
0      
1      
-1     
2      
-2     
23333333
-23333333
1234   

Integer Iteration 8
######0
######1
#####-1
######2
#####-2
23333333
-23333333
###1234

*** Output for binary type ***

 Input  numbers variation array is:
Array
(
    [0] => 0
    [1] => 1
    [2] => -1
    [3] => 2.7
    [4] => -2.7
    [5] => 23333333
    [6] => -23333333
    [7] => 1234
)

0
1
11111111111111111111111111111111
10
11111111111111111111111111111110
1011001000000100111010101
11111110100110111111011000101011
10011010010

*** Output for char type ***

 Input Characters variation array is:
Array
(
    [0] => a
    [1] => a
    [2] => 67
    [3] => -67
    [4] => 99
)

 
 
C
½
c

*** Output for scientific type ***

 Input numbers variation array is:
Array
(
    [0] => 0
    [1] => 1
    [2] => -1
    [3] => 2.7
    [4] => -2.7
    [5] => 23333333
    [6] => -23333333
    [7] => 1234
)

0.000000e+0
1.000000e+0
-1.000000e+0
2.700000e+0
-2.700000e+0
2.333333e+7
-2.333333e+7
1.234000e+3

*** Output for unsigned integer type ***

 Input Integer numbers variation array is:
Array
(
    [0] => 0
    [1] => 1
    [2] => -1
    [3] => 2.7
    [4] => -2.7
    [5] => 23333333
    [6] => -23333333
    [7] => 1234
)

0
1
4294967295
2
4294967294
23333333
4271633963
1234

*** Output for octal type ***

 Input numbers variation array is:
Array
(
    [0] => 0
    [1] => 1
    [2] => -1
    [3] => 2.7
    [4] => -2.7
    [5] => 23333333
    [6] => -23333333
    [7] => 1234
)

0
1
37777777777
2
37777777776
131004725
37646773053
2322

*** Output for hexadecimal type ***

 Input numbers variation array is:
Array
(
    [0] => 0
    [1] => 1
    [2] => -1
    [3] => 2.7
    [4] => -2.7
    [5] => 23333333
    [6] => -23333333
    [7] => 1234
)

0
1
ffffffff
2
fffffffe
16409d5
fe9bf62b
4d2

*** Output for string type ***

 Input Strings format variation array is:
Array
(
    [0] => %5s
    [1] => %-5s
    [2] => %05s
    [3] => %'#5s
)

 Input strings variation array is:
Array
(
    [0] => 
    [1] => abc
    [2] => aaa
)

     
  abc
  aaa
     
abc  
aaa  
00000
00abc
00aaa
#####
##abc
##aaa

*** Output for '%g' type ***

 Input format variation array is:
Array
(
    [0] => %g
    [1] => %.0g
    [2] => %+g
    [3] => %-g
    [4] => %-1.2g
    [5] => %+1.2g
    [6] => %G
    [7] => %.0G
    [8] => %+G
    [9] => %-G
    [10] => %-1.2G
    [11] => %+1.2G
)

123456
-123456
1.0e+5
-1.0e+5
+123456
-123456
123456
-123456
120000
-120000
+120000
-120000
123456
-123456
1.0E+5
-1.0E+5
+123456
-123456
123456
-123456
120000
-120000
+120000
-120000

*** Output for '%%%.2f' as the format parameter ***
%12345678900.00

*** Output for '%%' as the format parameter ***
%

*** Output for precision value more than maximum ***
12345678900.0000000000000000000000000000000000000000

*** Output for invalid width(-15) specifier ***
15s

*** Output for '%F' as the format parameter ***
12345678900.000000

*** Output for '%X' as the format parameter ***
C

*** Output  with no format parameter ***
12345

*** Output for multiple format parameters ***
12345  abcdefghjklmnpqrstuvwxyz  12345


*** Output for excess of mixed type arguments  ***
abcdefghjklmnpqrstuvwxyz

*** Output for string format parameter and integer type argument ***
12345

*** Output for integer format parameter and string type argument ***
0

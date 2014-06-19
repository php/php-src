--TEST--
sprintf %f #2
--INI--
precision=14
--FILE--
<?php
var_dump(sprintf("%.3F", 100.426));
var_dump(sprintf("%.2F", 100.426));
var_dump(sprintf("%d",   100.426));
var_dump(sprintf("%d",   100.9));
var_dump(sprintf("%o",   100.426));
var_dump(sprintf("%o",   100.9));

/* copy & paste from the docs */

/* example#1: Argument swapping */
$num = 100.1;
$location = "world";

$format = 'There are %d monkeys in the %s';
var_dump(sprintf($format, $num, $location));

/* example#2: Argument swapping */
$format = 'The %s contains %d monkeys';
var_dump(sprintf($format, $num, $location));

/* example#3: Argument swapping */
$format = 'The %2$s contains %1$d monkeys';
var_dump(sprintf($format, $num, $location));

/* example#4: Argument swapping */
$format = 'The %2$s contains %1$d monkeys.
    That\'s a nice %2$s full of %1$d monkeys.';
var_dump(sprintf($format, $num, $location));

/* example#5: various examples */
$n =  43951789;
$u = -43951789;
$c = 65; // ASCII 65 is 'A'

// notice the double %%, this prints a literal '%' character
var_dump(sprintf("%%b = '%b'", $n)); // binary representation
var_dump(sprintf("%%c = '%c'", $c)); // print the ascii character, same as chr() function
var_dump(sprintf("%%d = '%d'", $n)); // standard integer representation
var_dump(sprintf("%%e = '%e'", $n)); // scientific notation
var_dump(sprintf("%%u = '%u'", $n)); // unsigned integer representation of a positive integer
var_dump(sprintf("%%u = '%u'", $u)); // unsigned integer representation of a negative integer
var_dump(sprintf("%%f = '%f'", $n)); // floating point representation
var_dump(sprintf("%%o = '%o'", $n)); // octal representation
var_dump(sprintf("%%s = '%s'", $n)); // string representation
var_dump(sprintf("%%x = '%x'", $n)); // hexadecimal representation (lower-case)
var_dump(sprintf("%%X = '%X'", $n)); // hexadecimal representation (upper-case)

var_dump(sprintf("%%+d = '%+d'", $n)); // sign specifier on a positive integer
var_dump(sprintf("%%+d = '%+d'", $u)); // sign specifier on a negative integer


/* example#6: string specifiers */
$s = 'monkey';
$t = 'many monkeys';

var_dump(sprintf("[%s]",      $s)); // standard string output
var_dump(sprintf("[%10s]",    $s)); // right-justification with spaces
var_dump(sprintf("[%-10s]",   $s)); // left-justification with spaces
var_dump(sprintf("[%010s]",   $s)); // zero-padding works on strings too
var_dump(sprintf("[%'#10s]",  $s)); // use the custom padding character '#'
var_dump(sprintf("[%10.10s]", $t)); // left-justification but with a cutoff of 10 characters

/* example#7: zero-padded integers */
var_dump(sprintf("%04d-%02d-%02d", 2006, 12, 18));

/* example#8: formatting currency */
$money1 = 68.75;
$money2 = 54.35;
$money = $money1 + $money2;
var_dump(sprintf("%01.2f", $money)); // output "123.10"

/* example#9: scientific notation */
$number = 362525200;
 
var_dump(sprintf("%.3e", $number)); // outputs 3.63e+8
?>
--EXPECTREGEX--
string\(7\) \"100\.426\"
string\(6\) \"100\.43\"
string\(3\) \"100\"
string\(3\) \"100\"
string\(3\) \"144\"
string\(3\) \"144\"
string\(34\) \"There are 100 monkeys in the world\"
string\(28\) \"The 100\.1 contains 0 monkeys\"
string\(30\) \"The world contains 100 monkeys\"
string\(76\) \"The world contains 100 monkeys.
    That's a nice world full of 100 monkeys\.\"
string\(33\) \"%b = '10100111101010011010101101'\"
string\(8\) \"%c = 'A'\"
string\(15\) \"%d = '43951789'\"
string\(18\) \"%e = '4\.395179e\+7'\"
string\(15\) \"%u = '43951789'\"
(string\(17\) \"%u = '4251015507'\"|string\(27\) \"%u = '18446744073665599827'\")
string\(22\) \"%f = '43951789\.000000'\"
string\(16\) \"%o = '247523255'\"
string\(15\) \"%s = '43951789'\"
string\(14\) \"%x = '29ea6ad'\"
string\(14\) \"%X = '29EA6AD'\"
string\(17\) \"%\+d = '\+43951789'\"
string\(17\) \"%\+d = '-43951789'\"
string\(8\) \"\[monkey\]\"
string\(12\) \"\[    monkey\]\"
string\(12\) \"\[monkey    \]\"
string\(12\) \"\[0000monkey\]\"
string\(12\) \"\[####monkey\]\"
string\(12\) \"\[many monke\]\"
string\(10\) \"2006-12-18\"
string\(6\) \"123\.10\"
string\(8\) \"3\.625e\+8\"

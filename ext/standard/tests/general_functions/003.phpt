--TEST--
levenshtein() function test
--FILE--
<?php

function test_me($title,$expect,$text1,$text2,$cost1=0,$cost2=0,$cost3=0) {

  if($cost1==0)
	  $result=levenshtein($text1,$text2);
	else
    $result=levenshtein($text1,$text2,$cost1,$cost2,$cost3);

	if($result==$expect) return 0;

	echo "$title: result is $result instead of $expect ";
	echo "for '$text1'/'$text2' ";
	if($cost1) echo "($cost1:$cost2:$cost3)";
	echo "\n";

	return 1;
}

$n=0;

$n += test_me("equal"      , 0, "12345", "12345");
$n += test_me("1st empty"  , 3,      "",   "xzy");
$n += test_me("2nd empty"  , 3,   "xzy",      "");
$n += test_me("both empty" , 0,      "",      "");
$n += test_me("1 char"     , 1,     "1",     "2");
$n += test_me("2 char swap", 2,    "12",    "21");

$n += test_me("inexpensive delete",  2, "2121",   "11", 2, 1, 1);
$n += test_me("expensive delete"  , 10, "2121",   "11", 2, 1, 5);
$n += test_me("inexpensive insert",  2,   "11", "2121", 1, 1, 1);
$n += test_me("expensive insert"  , 10,   "11", "2121", 5, 1, 1);

$n += test_me("expensive replace"     , 3, "111", "121", 2, 3, 2);
$n += test_me("very expensive replace", 4, "111", "121", 2, 9, 2);

$n += test_me("bug #7368", 2,     "13458",    "12345");
$n += test_me("bug #7368", 2,      "1345",     "1234");

$n += test_me("bug #6562", 1,    "debugg",    "debug");
$n += test_me("bug #6562", 1,    "ddebug",    "debug");
$n += test_me("bug #6562", 2,   "debbbug",    "debug");
$n += test_me("bug #6562", 1, "debugging", "debuging");

$n += test_me("bug #16473", 2, "a", "bc");
$n += test_me("bug #16473", 2, "xa", "xbc");
$n += test_me("bug #16473", 2, "xax", "xbcx");
$n += test_me("bug #16473", 2, "ax", "bcx");


echo ($n==0)?"all passed\n":"$n failed\n";

?>
--EXPECT--
all passed

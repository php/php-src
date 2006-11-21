--TEST--
levenshtein() function test
--FILE--
<?php 
 
function test_me($title,$expect,$text1,$text2,$cost1="",$cost2="",$cost3="") {
 
  if ($cost1=="") { 
          $result=levenshtein($text1,$text2);
  }
  elseif ($cost2=="") {
          $result=levenshtein($text1,$text2,$cost1);
  }
  else  {
    $result=levenshtein($text1,$text2,$cost1,$cost2,$cost3);
  }
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
 
$n += test_me("custom", -1, "111", "121", "my_levcode");
$n += test_me("lt maxlength1", 254, "AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsu", "A");
$n += test_me("gt maxlength1", -1, "AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuv", "A");
 
$n += test_me("lt maxlength2", 254, "A", "AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsu");
$n += test_me("gt maxlength2", -1, "A",  "AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuv");
 
echo ($n==0)?"all passed\n":"$n failed\n";

var_dump(levenshtein(array(), array()));
var_dump(levenshtein("", "", -1, -1, -1));
var_dump(levenshtein("", "", 10, 10, 10));
var_dump(levenshtein(""));

?>
--EXPECTF--
Warning: levenshtein(): The general Levenshtein support is not there yet in %s on line %d

Warning: levenshtein(): Argument string(s) too long in %s on line %d

Warning: levenshtein(): Argument string(s) too long in %s on line %d
all passed

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
int(0)
int(0)
int(0)

Warning: Wrong parameter count for levenshtein() in %s on line %d
NULL

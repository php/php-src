--TEST--
similar_text(), basic tests
--CREDITS--
Mats Lindh <mats at lindh.no>
#Testfest php.no
--INI--
precision=14
--FILE--
<?php
/* Prototype  : proto int similar_text(string str1, string str2 [, float percent])
* Description: Calculates the similarity between two strings
* Source code: ext/standard/string.c
*/
echo "\n-- Basic calls with just two strings --\n"; 
var_dump(similar_text("abcdefgh", "efg"));
var_dump(similar_text("abcdefgh", "mno"));
var_dump(similar_text("abcdefghcc", "c"));
var_dump(similar_text("abcdefghabcdef", "zzzzabcdefggg"));

echo "\n-- Same calls with precentage return variable specified --\n"; 
$percent = 0;
similar_text("abcdefgh", "efg", $percent);
var_dump($percent);
similar_text("abcdefgh", "mno", $percent);
var_dump($percent);
similar_text("abcdefghcc", "c", $percent);
var_dump($percent);
similar_text("abcdefghabcdef", "zzzzabcdefggg", $percent);
var_dump($percent);
?>
===DONE===
--EXPECTF--
-- Basic calls with just two strings --
int(3)
int(0)
int(1)
int(7)

-- Same calls with precentage return variable specified --
float(54.545454545455)
float(0)
float(18.181818181818)
float(51.851851851852)
===DONE===

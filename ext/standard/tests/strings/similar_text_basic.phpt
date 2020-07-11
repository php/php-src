--TEST--
similar_text(), basic tests
--CREDITS--
Mats Lindh <mats at lindh.no>
#Testfest php.no
--INI--
precision=14
--FILE--
<?php
var_dump(similar_text("abcdefgh", "efg"));
var_dump(similar_text("abcdefgh", "mno"));
var_dump(similar_text("abcdefghcc", "c"));
var_dump(similar_text("abcdefghabcdef", "zzzzabcdefggg"));

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
--EXPECT--
int(3)
int(0)
int(1)
int(7)
float(54.54545454545455)
float(0)
float(18.181818181818183)
float(51.851851851851855)

--TEST--
GH-20836 (Stack overflow in mb_convert_variables with recursive array references)
--EXTENSIONS--
mbstring
--FILE--
<?php

$a = [];
$b = [];
$b[] = &$a;
$a[] = &$b;
var_dump(mb_convert_variables('utf-8', 'utf-8', $a));

$c = [];
$c[] = &$c;
var_dump(mb_convert_variables('utf-8', 'utf-8', $c));

$normal = ['test', 'array'];
var_dump(mb_convert_variables('utf-8', 'utf-8', $normal));

$d = ['level1' => ['level2' => ['level3' => 'data']]];
var_dump(mb_convert_variables('utf-8', 'utf-8', $d));

echo "Done\n";
?>
--EXPECTF--
Warning: mb_convert_variables(): Cannot handle recursive references in %s on line %d
bool(false)

Warning: mb_convert_variables(): Cannot handle recursive references in %s on line %d
bool(false)
string(5) "UTF-8"
string(5) "UTF-8"
Done

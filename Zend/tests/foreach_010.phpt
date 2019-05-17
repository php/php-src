--TEST--
Nested foreach by value over object and object modification with resize
--FILE--
<?php
$o = (object)['a'=>0, 'b'=>1, 'c'=>2, 'd'=>3, 'e'=>4, 'f'=>5, 'g'=>6, 'h'=>7];
unset($o->a, $o->b, $o->c, $o->d);
foreach ($o as $v1) {
	foreach ($o as $v2) {
		echo "$v1-$v2\n";
		if ($v1 == 5 && $v2 == 6) {
			$o->i = 8;
		}
	}
}
?>
--EXPECT--
4-4
4-5
4-6
4-7
5-4
5-5
5-6
5-7
5-8
6-4
6-5
6-6
6-7
6-8
7-4
7-5
7-6
7-7
7-8
8-4
8-5
8-6
8-7
8-8

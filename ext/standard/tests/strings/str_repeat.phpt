--TEST--
str_repeat() function
--POST--
--GET--
--FILE--
<?php
$input = array('a', 'foo', 'barbazbax');
foreach($input AS $i) {
	for($n=0; $n<5; $n++) {
		echo str_repeat($i, $n)."\n";
	}
}
?>
--EXPECT--

a
aa
aaa
aaaa

foo
foofoo
foofoofoo
foofoofoofoo

barbazbax
barbazbaxbarbazbax
barbazbaxbarbazbaxbarbazbax
barbazbaxbarbazbaxbarbazbaxbarbazbax

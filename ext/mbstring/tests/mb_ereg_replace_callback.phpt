--TEST--
mb_ereg_replace_callback()
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace_callback') or die("skip mb_ereg_replace_callback() is not available in this build");
?>
--FILE--
<?php
$str = 'abc 123 #",; $foo';

echo mb_ereg_replace_callback('(\S+)', function ($m) {
    return $m[1].'('.strlen($m[1]).')';
}, $str), "\n";

echo mb_ereg_replace_callback('(?<word>\w+) (?<digit>\d+).*', function ($m) {
    return sprintf("%s-%s", $m['digit'], $m['word']);
}, $str), "\n";
?>
--EXPECT--
abc(3) 123(3) #",;(4) $foo(4)
123-abc

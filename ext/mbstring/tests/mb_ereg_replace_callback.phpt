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
--EXPECTF--
Deprecated: Function mb_ereg_replace_callback() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
abc(3) 123(3) #",;(4) $foo(4)

Deprecated: Function mb_ereg_replace_callback() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
123-abc

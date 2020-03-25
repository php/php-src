--TEST--
Bug #72918 (negative offset inside a quoted string leads to parse error)
--FILE--
<?php
$array = [-3 => 'foo'];
$string = 'abcde';

echo "$array[-3]\n";
echo "$string[-3]\n";
echo <<<EOT
$array[-3]
$string[-3]

EOT;
?>
--EXPECT--
foo
c
foo
c

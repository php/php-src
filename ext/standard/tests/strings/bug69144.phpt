--TEST--
Bug #69144 (strtr not replacing with partly matching replace pairs)
--FILE--
<?php
$tests = array('bar' => '', 'foo' => 'o', 'foobar' => '', 'hello' => 'hello');

foreach ($tests as $input => $expected) {
    if ($expected !== ($actual = strtr($input, array("fo" => "", "foobar" => "", "bar" => "")))) {
        echo "KO `$input` became `$actual` instead of `$expected`\n";
    }
}
echo "okey";
?>
--EXPECT--
okey

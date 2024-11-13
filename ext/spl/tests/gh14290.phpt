--TEST--
GH-14290 (Member access within null pointer in extension spl)
--INI--
pcre.backtrack_limit=2
pcre.jit=0
--FILE--
<?php
$h = new ArrayIterator(['test' => 'test1']);
$i = new RegexIterator($h, '/^test(.*)/', RegexIterator::REPLACE);
foreach ($i as $name => $value) {
    var_dump($name, $value);
}
echo "Done\n";
?>
--EXPECT--
Done

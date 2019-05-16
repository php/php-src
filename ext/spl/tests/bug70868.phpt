--TEST--
Bug #70868, with PCRE JIT
--INI--
pcre.jit=1
--SKIPIF--
<?php if (!extension_loaded("pcre")) die("skip"); ?>
--FILE--
<?php

namespace X;

$iterator =
    new \RegexIterator(
        new \ArrayIterator(['A.phpt', 'B.phpt', 'C.phpt']),
        '/\.phpt$/'
    )
;

foreach ($iterator as $foo) {
    var_dump($foo);
    preg_replace('/\.phpt$/', '', '');
}

echo "Done", PHP_EOL;

?>
--EXPECT--
string(6) "A.phpt"
string(6) "B.phpt"
string(6) "C.phpt"
Done

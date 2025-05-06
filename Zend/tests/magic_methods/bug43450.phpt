--TEST--
Bug #43450 (Memory leak on some functions with implicit object __toString() call)
--INI--
opcache.enable_cli=0
--FILE--
<?php

class Foo
{
    public function __toString()
    {
        return __CLASS__;
    }
}

$num_repeats = 100000;

$start = memory_get_usage() / 1024;
for ($i=1;$i<$num_repeats;$i++)
{
    $foo = new Foo();
    md5($foo);
}
$end = memory_get_usage() / 1024;

if ($start + 16 < $end) {
    echo 'FAIL';
} else {
    echo 'PASS';
}

?>
--EXPECT--
PASS

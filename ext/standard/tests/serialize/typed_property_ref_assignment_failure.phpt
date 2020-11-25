--TEST--
Failure to assign ref to typed property
--FILE--
<?php

class Test {
    public int $prop;
}

$s = <<<'STR'
O:4:"Test":1:{s:4:"prop";O:8:"stdClass":1:{s:1:"y";R:2;}}
STR;
try {
    var_dump(unserialize($s));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot assign stdClass to property Test::$prop of type int

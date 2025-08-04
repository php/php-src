--TEST--
Frameless bug 001
--FILE--
<?php

namespace NS;

class Test {
    public function test() {
        return preg_match('foo', 'bar');
    }
}

$test = new Test();
$test->test();

?>
--EXPECTF--
Warning: preg_match(): Delimiter must not be alphanumeric, backslash, or NUL byte in %s on line %d

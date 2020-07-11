--TEST--
Using procedural finfo API in a method
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

class Test {
    public function method() {
        $finfo = finfo_open(FILEINFO_MIME);
        var_dump(finfo_file($finfo, __FILE__));
    }
}

$test = new Test;
$test->method();

?>
--EXPECT--
string(28) "text/x-php; charset=us-ascii"

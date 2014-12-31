--TEST--
ISSUE #79 (Optimization Problem/Bug)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class Test {
    public function run() {
        $r = $this->my_parse_m();
        var_dump ($r);
        return $r;
    }

    public function my_parse_m() {
        $test = true;
        if ($test === true) {
            $a = 'b';
        } else {
            return false;
        }
//      flush();
        return true;
    }
}

$t = new Test();
var_dump ($t->run());
--EXPECT--
bool(true)
bool(true)

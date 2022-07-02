--TEST--
Testing variable variables as function name
--FILE--
<?php

$a = 'ucfirst';
$b = 'a';
print $$b('test');
print "\n";


class bar {
    public function a() {
        return "bar!";
    }
}

class foo {
    public function test() {
        print "foo!\n";
        return new bar;
    }
}

function test() {
    return new foo;
}

$a = 'test';
$b = 'a';
var_dump($$b()->$$b()->$b());


$a = 'strtoupper';
$b = 'a';
$c = 'b';
$d = 'c';
var_dump($$$$d('foo'));

?>
--EXPECT--
Test
foo!
string(4) "bar!"
string(3) "FOO"

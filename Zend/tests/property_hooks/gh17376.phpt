--TEST--
GH-17376: Child classes may add hooks to plain properties
--INI--
# Avoid triggering for main, trigger for test so we get a side-trace for property hooks
opcache.jit_hot_func=2
--FILE--
<?php

class A {
    public $prop = 1;
}

class B extends A {
    public $prop = 1 {
        get {
            echo __METHOD__, "\n";
            return $this->prop;
        }
        set {
            echo __METHOD__, "\n";
            $this->prop = $value;
        }
    }
}

function test(A $a) {
    echo "read\n";
    var_dump($a->prop);
    echo "write\n";
    $a->prop = 42;
    echo "read-write\n";
    $a->prop += 43;
    echo "pre-inc\n";
    ++$a->prop;
    echo "pre-dec\n";
    --$a->prop;
    echo "post-inc\n";
    $a->prop++;
    echo "post-dec\n";
    $a->prop--;
}

echo "A\n";
test(new A);

echo "\nA\n";
test(new A);

echo "\nB\n";
test(new B);

echo "\nB\n";
test(new B);

?>
--EXPECT--
A
read
int(1)
write
read-write
pre-inc
pre-dec
post-inc
post-dec

A
read
int(1)
write
read-write
pre-inc
pre-dec
post-inc
post-dec

B
read
B::$prop::get
int(1)
write
B::$prop::set
read-write
B::$prop::get
B::$prop::set
pre-inc
B::$prop::get
B::$prop::set
pre-dec
B::$prop::get
B::$prop::set
post-inc
B::$prop::get
B::$prop::set
post-dec
B::$prop::get
B::$prop::set

B
read
B::$prop::get
int(1)
write
B::$prop::set
read-write
B::$prop::get
B::$prop::set
pre-inc
B::$prop::get
B::$prop::set
pre-dec
B::$prop::get
B::$prop::set
post-inc
B::$prop::get
B::$prop::set
post-dec
B::$prop::get
B::$prop::set

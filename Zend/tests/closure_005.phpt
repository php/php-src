--TEST--
Closure 005: Lambda inside class, lifetime of $this
--FILE--
<?php

class A {
    private $x;

    function __construct($x) {
        $this->x = $x;
    }

    function __destruct() {
        echo "Destroyed\n";
    }

    function getIncer($val) {
        return function() use ($val) {
            $this->x += $val;
        };
    }

    function getPrinter() {
        return function() {
            echo $this->x."\n";
        };
    }

    function getError() {
        return static function() {
            echo $this->x."\n";
        };
    }

    function printX() {
        echo $this->x."\n";
    }
}

$a = new A(3);
$incer = $a->getIncer(2);
$printer = $a->getPrinter();
$error = $a->getError();

$a->printX();
$printer();
$incer();
$a->printX();
$printer();

unset($a);

$incer();
$printer();

unset($incer);
$printer();

unset($printer);

$error();

echo "Done\n";
?>
--EXPECTF--
3
3
5
5
7
7
Destroyed

Fatal error: Uncaught Error: Using $this when not in object context in %sclosure_005.php:28
Stack trace:
#0 %s(%d): A::{closure}()
#1 {main}
  thrown in %sclosure_005.php on line 28

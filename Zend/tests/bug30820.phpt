--TEST--
Bug #30820 (static member conflict with $this->member silently ignored)
--INI--
error_reporting=4095
--FILE--
<?php
class Blah {
    private static $x;

    public function show() {
        Blah::$x = 1;
        $this->x = 5; // no warning, but refers to different variable

        echo 'Blah::$x = '. Blah::$x ."\n";
        echo '$this->x = '. $this->x ."\n";
    }
}

$b = new Blah();
$b->show();
?>
--EXPECTF--
Strict Standards: Accessing static property Blah::$x as non static in %sbug30820.php on line 7
Blah::$x = 1

Strict Standards: Accessing static property Blah::$x as non static in %sbug30820.php on line 10
$this->x = 5

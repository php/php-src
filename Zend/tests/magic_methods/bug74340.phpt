--TEST--
Bug #74340: Magic function __get has different behavior in php 7.1.x
--FILE--
<?php
class Test
{
    public function __get($var)
    {
        static $first = true;
        echo '__get '.$var.PHP_EOL;
        if ($first) {
            $first = false;
            $this->$var;
            $this->{$var.'2'};
            $this->$var;
        }
    }
}

$test = new Test;
$test->test;

?>
--EXPECTF--
__get test

Warning: Undefined property: Test::$test in %s on line %d
__get test2

Warning: Undefined property: Test::$test in %s on line %d

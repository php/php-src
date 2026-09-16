--TEST--
Handling of assign-ops and incdecs on overloaded properties using &__get()
--FILE--
<?php

class Test {
    protected $a = 0;
    protected $b = 0;
    protected $c = 0;

    public function &__get($name) {
        echo "get($name)\n";
        return $this->$name;
    }

    public function __set($name, $value) {
        echo "set($name, $value)\n";
    }
}

$test = new Test;

var_dump($test->a += 1);
var_dump($test->b++);
var_dump(++$test->c);

var_dump($test);

?>
--EXPECT--
get(a)
set(a, 1)
int(1)
get(b)
set(b, 1)
int(0)
get(c)
set(c, 1)
int(1)
object(Test)#1 (3) {
  ["a":protected]=>
  int(0)
  ["b":protected]=>
  int(0)
  ["c":protected]=>
  int(0)
}

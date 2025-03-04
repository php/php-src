--TEST--
inner references
--FILE--
<?php

class Outer {
    private class Inner(public int $x);

    public function __construct(int $x) {
        var_dump(new static::Inner($x));
    }
}

new Outer(0);
?>
--EXPECT--
object(Outer::Inner)#2 (1) {
  ["x"]=>
  int(0)
}

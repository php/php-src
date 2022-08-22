--TEST--
clone can write to readonly properties
--FILE--
<?php

class Counter
{
    private static int $counter = 0;

    public readonly int $count;
    private readonly int $foo;

    public function __construct()
    {
        $this->count = ++self::$counter;
        $this->foo = 0;
    }

    public function count(?int $count = null): static
    {
        $new = clone $this;
        $new->count = $count ?? ++self::$counter;

        return $new;
    }

    public function __clone()
    {
        if (is_a(debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 2)[1]['class'] ?? '', self::class, true)) {
            unset($this->count);
        } else {
            $this->count = ++self::$counter;
        }
        $this->foo = 1;
    }
}

$a = new Counter();
var_dump($a);

var_dump(clone $a);

$b = $a->count();
var_dump($b);

$c = $a->count(123);
var_dump($c);

?>
--EXPECTF--
object(Counter)#%d (2) {
  ["count"]=>
  int(1)
  ["foo":"Counter":private]=>
  int(0)
}
object(Counter)#%d (2) {
  ["count"]=>
  int(2)
  ["foo":"Counter":private]=>
  int(1)
}
object(Counter)#%d (2) {
  ["count"]=>
  int(3)
  ["foo":"Counter":private]=>
  int(1)
}
object(Counter)#%d (2) {
  ["count"]=>
  int(123)
  ["foo":"Counter":private]=>
  int(1)
}

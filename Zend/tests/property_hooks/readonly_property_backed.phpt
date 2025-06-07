--TEST--
Backed readonly property may have hooks
--FILE--
<?php

// readonly property
class Test {
    public readonly int $prop {
        get => $this->prop;
        set => $value;
    }

    public function __construct(int $v) {
        $this->prop = $v;
    }

    public function set($v)
    {
         $this->prop = $v;
    }
}

$t = new Test(42);
var_dump($t->prop);
try {
    $t->set(43);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $t->prop = 43;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($t->prop);

// class readonly
final readonly class Foo
{
    public function __construct(
        public array $values {
            set(array $value) => array_map(strtoupper(...), $value);
        },
    ) {}
}

// property readonly
final class Foo2
{
    public function __construct(
        public readonly array $values {
            set(array $value) => array_map(strtoupper(...), $value);
        },
    ) {}
}

// redundant readonly
final readonly class Foo3
{
    public function __construct(
        public readonly array $values {
            set(array $value) => array_map(strtoupper(...), $value);
            get => $this->makeNicer($this->values);
        },
    ) {}

    public function makeNicer(array $entries): array
    {
      return array_map(
        fn($i, $entry) => $entry . strtoupper(['', 'r', 'st'][$i]), array_keys($entries),
        $entries
       );
    }
}

\var_dump(new Foo(['yo,', 'you', 'can'])->values);
\var_dump(new Foo2(['just', 'do', 'things'])->values);
\var_dump(new Foo3(['nice', 'nice', 'nice'])->values);
?>
--EXPECT--
int(42)
Cannot modify readonly property Test::$prop
Cannot modify protected(set) readonly property Test::$prop from global scope
int(42)
array(3) {
  [0]=>
  string(3) "YO,"
  [1]=>
  string(3) "YOU"
  [2]=>
  string(3) "CAN"
}
array(3) {
  [0]=>
  string(4) "JUST"
  [1]=>
  string(2) "DO"
  [2]=>
  string(6) "THINGS"
}
array(3) {
  [0]=>
  string(4) "NICE"
  [1]=>
  string(5) "NICER"
  [2]=>
  string(6) "NICEST"
}
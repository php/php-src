--TEST--
__exists: empty() short-circuits on __exists=false; otherwise calls __get for truthiness
--FILE--
<?php

class C {
    public ?string $stored = null;
    public bool $hasIt;
    public function __construct(bool $hasIt, ?string $stored = null) {
        $this->hasIt = $hasIt;
        $this->stored = $stored;
    }
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        return $this->hasIt;
    }
    public function __get(string $n): mixed {
        echo "  __get($n)\n";
        return $this->stored;
    }
}

echo "1) empty() when __exists=false: only __exists, returns true:\n";
$c = new C(false);
var_dump(empty($c->any));

echo "\n2) empty() when __exists=true and __get returns null: returns true:\n";
$c = new C(true, null);
var_dump(empty($c->any));

echo "\n3) empty() when __exists=true and __get returns '': returns true:\n";
$c = new C(true, '');
var_dump(empty($c->any));

echo "\n4) empty() when __exists=true and __get returns 'x': returns false:\n";
$c = new C(true, 'x');
var_dump(empty($c->any));

?>
--EXPECT--
1) empty() when __exists=false: only __exists, returns true:
  __exists(any)
bool(true)

2) empty() when __exists=true and __get returns null: returns true:
  __exists(any)
  __get(any)
bool(true)

3) empty() when __exists=true and __get returns '': returns true:
  __exists(any)
  __get(any)
bool(true)

4) empty() when __exists=true and __get returns 'x': returns false:
  __exists(any)
  __get(any)
bool(false)

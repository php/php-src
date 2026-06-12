--TEST--
__exists: when both __exists and __isset are defined, __exists wins; __isset is never called
--FILE--
<?php

class C {
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        return true;
    }
    public function __isset(string $n): bool {
        throw new Exception("__isset must not be called when __exists is defined");
    }
    public function __get(string $n): mixed {
        echo "  __get($n)\n";
        return 'g';
    }
}

echo "1) `??`:\n";
$c = new C; var_dump($c->x ?? 'fb');

echo "\n2) isset():\n";
$c = new C; var_dump(isset($c->x));

echo "\n3) empty():\n";
$c = new C; var_dump(empty($c->x));

?>
--EXPECT--
1) `??`:
  __exists(x)
  __get(x)
string(1) "g"

2) isset():
  __exists(x)
  __get(x)
bool(true)

3) empty():
  __exists(x)
  __get(x)
bool(false)

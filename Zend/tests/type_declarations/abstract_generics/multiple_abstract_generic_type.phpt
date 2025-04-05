--TEST--
Multiple abstract generic type
--FILE--
<?php

interface I<K, V> {
    public function set(K $key, V $value): void;
    public function get(K $key): V;
}

class C1 implements I<int, string> {
    public array $a = [];
    public function set(int $key, string $value): void {
        $this->a[$key] = $value . '!';
    }
    public function get(int $key): string {
        return $this->a[$key];
    }
}

class C2 implements I<string, object> {
    public array $a = [];
    public function set(string $key, object $value): void {
        $this->a[$key] = $value;
    }
    public function get(string $key): object {
        return $this->a[$key];
    }
}

$c1 = new C1();
$c1->set(5, "Hello");
var_dump($c1->a);
var_dump($c1->get(5));

$c2 = new C2();
$c2->set('C1', $c1);
var_dump($c2->a);
var_dump($c2->get('C1'));

try {
    $c1->set('blah', "Hello");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}


?>
--EXPECTF--
array(1) {
  [5]=>
  string(6) "Hello!"
}
string(6) "Hello!"
array(1) {
  ["C1"]=>
  object(C1)#1 (1) {
    ["a"]=>
    array(1) {
      [5]=>
      string(6) "Hello!"
    }
  }
}
object(C1)#1 (1) {
  ["a"]=>
  array(1) {
    [5]=>
    string(6) "Hello!"
  }
}
TypeError: C1::set(): Argument #1 ($key) must be of type int, string given, called in %s on line %d

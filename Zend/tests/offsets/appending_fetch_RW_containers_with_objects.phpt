--TEST--
Appending containers via a binary RW op $c[] .= $v; fetch is an object
--EXTENSIONS--
gmp
--FILE--
<?php

class FakeString {
    public function __toString(): string {
        return "Hello ";
    }
}

class TestFetchAppendStringableObject implements FetchAppendable {
    public $str;
    public function append(mixed $v): void {}
    public function &fetchAppend(): object {
        $this->str = new FakeString();
        return $this->str;
    }
}

$container = new TestFetchAppendStringableObject();
try {
    $container[] .= 'value';
    var_dump($container);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

class TestFetchAppendStringableObjectTypedProperty implements FetchAppendable {
    public object $str;
    public function append(mixed $v): void {}
    public function &fetchAppend(): object {
        $this->str = new FakeString();
        return $this->str;
    }
}

$container = new TestFetchAppendStringableObjectTypedProperty();
try {
    $container[] .= 'value';
    var_dump($container);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

class TestFetchAppendObject implements FetchAppendable {
    public object $n;
    public function append(mixed $v): void {}
    public function &fetchAppend(): object {
        $this->n = gmp_init(20);
        return $this->n;
    }
}

$container = new TestFetchAppendObject();
try {
    $container[] += 22;
    var_dump($container);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
object(TestFetchAppendStringableObject)#1 (1) {
  ["str"]=>
  string(11) "Hello value"
}
TypeError: Cannot assign string to reference held by property TestFetchAppendStringableObjectTypedProperty::$str of type object
object(TestFetchAppendObject)#4 (1) {
  ["n"]=>
  object(GMP)#1 (1) {
    ["num"]=>
    string(2) "42"
  }
}

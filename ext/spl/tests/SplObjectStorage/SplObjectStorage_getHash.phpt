--TEST--
SplObjectStorage::getHash() implementation
--FILE--
<?php

class MySplObjectStorage1 extends SplObjectStorage {
    #[ReturnTypeWillChange]
    public function getHash($obj) {
        return 2;
    }
}

class MySplObjectStorage2 extends SplObjectStorage {
    public function getHash($obj): string {
        throw new Exception("foo");
        return "asd";
    }
}

class MySplObjectStorage3 extends SplObjectStorage {
    public function getHash($obj): string {
        return "asd";
    }
}

$s = new SplObjectStorage();
$o1 = new stdClass();
$o2 = new stdClass();

$instances = [
    new SplObjectStorage(),
    new MySplObjectStorage1(),
    new MySplObjectStorage2(),
    new MySplObjectStorage3(),
];

foreach ($instances as $instance) {
    echo 'Instance as ', $instance::class, PHP_EOL;
    try {
        $instance[$o1] = 'foo';
        var_dump($instance->offsetGet($o1));
        var_dump($instance[$o1]);
        $instance[$o2] = $o2;
        var_dump($instance[$o1] === $instance[$o2]);
    } catch(Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
Instance as SplObjectStorage
string(3) "foo"
string(3) "foo"
bool(false)
Instance as MySplObjectStorage1
TypeError: MySplObjectStorage1::getHash(): Return value must be of type string, int returned
Instance as MySplObjectStorage2
Exception: foo
Instance as MySplObjectStorage3
string(3) "foo"
string(3) "foo"
bool(true)

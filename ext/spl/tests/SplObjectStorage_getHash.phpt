--TEST--
SplObjectStorage::getHash implementation
--FILE--
<?php
$s = new SplObjectStorage();
$o1 = new Stdclass;
$o2 = new Stdclass;
$s[$o1] = "some_value\n";
var_dump($s->offsetGet($o1));

class MySplObjectStorage extends SplObjectStorage {
    public function getHash($obj) {
        return 2;
    }
}

try {
    $s1 = new MySplObjectStorage;
    $s1[$o1] = "foo";
} catch (\TypeError $e) {
    echo $e->getMessage() . PHP_EOL;
}

class MySplObjectStorage2 extends SplObjectStorage {
    public function getHash($obj) {
        throw new Exception("foo");
        return "asd";
    }
}

try {
    $s2 = new MySplObjectStorage2;
    $s2[$o2] = "foo";
} catch (\Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

class MySplObjectStorage3 extends SplObjectStorage {
    public function getHash($obj) {
        return "asd";
    }
}

$s3 = new MySplObjectStorage3;
$s3[$o1] = $o1;
var_dump($s3[$o1]);
$s3[$o2] = $o2;

var_dump($s3[$o1] === $s3[$o2]);

?>
--EXPECTF--
string(11) "some_value
"
Hash needs to be a string
foo
object(stdClass)#%d (0) {
}
bool(true)

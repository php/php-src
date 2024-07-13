--TEST--
Auto-vivification for objects
--FILE--
<?php

class Vector implements DimensionFetchable, DimensionWritable, FetchAppendable {
    private array $a = [];

    private function dumpCall($method): void {
        echo 'Object ID #', spl_object_id($this), ' called ', $method, PHP_EOL;
    }

    public function offsetExists($offset): bool {
        $this->dumpCall(__METHOD__);
        return array_key_exists($offset, $this->a);
    }
    public function offsetGet($offset): mixed {
        $this->dumpCall(__METHOD__);
        return $this->a[$offset];
    }
    public function offsetSet($offset, $value): void {
        $this->dumpCall(__METHOD__);
        $this->a[$offset] = $value;
    }
    public function &offsetFetch($offset): mixed {
        $this->dumpCall(__METHOD__);
        return $this->a[$offset];
    }
    public function append($value): void {
        $this->dumpCall(__METHOD__);
        $this->a[] = $value;
    }
    public function &fetchAppend(): mixed {
        $this->dumpCall(__METHOD__);
        $ref =& $this->a[];
        return $ref;
    }
}

$obj = new Vector();

$ref = &$obj[0];
var_dump($ref);

$obj[1][] = 'append';
$obj[2][50] = 'set-to-offset';

echo "Check results\n";
var_dump($obj[1]);
var_dump($obj[2]);

?>
--EXPECT--
Object ID #1 called Vector::offsetFetch
NULL
Object ID #1 called Vector::offsetFetch
Object ID #2 called Vector::append
Object ID #1 called Vector::offsetFetch
Object ID #3 called Vector::offsetSet
Check results
Object ID #1 called Vector::offsetGet
object(Vector)#2 (1) {
  ["a":"Vector":private]=>
  array(1) {
    [0]=>
    string(6) "append"
  }
}
Object ID #1 called Vector::offsetGet
object(Vector)#3 (1) {
  ["a":"Vector":private]=>
  array(1) {
    [50]=>
    string(13) "set-to-offset"
  }
}

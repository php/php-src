--TEST--
ArrayObject and ArrayIterator written to while their storage is being encoded, sorted or compared
--FILE--
<?php
class Writer implements JsonSerializable
{
    public function __construct(private object $target) {}

    public function jsonSerialize(): mixed
    {
        for ($i = 0; $i < 20; $i++) {
            $this->target["new$i"] = $i;
        }
        unset($this->target['b']);
        $ref = &$this->target['ref'];
        $ref = 'ref';

        return 'written';
    }

    public function __toString(): string
    {
        $this->target[] = 'appended';

        return 'x';
    }
}

echo "-- json_encode()\n";
foreach (['ArrayObject', 'ArrayIterator'] as $class) {
    $object = new $class(['a' => null, 'b' => 2, 'c' => 3]);
    $object['a'] = new Writer($object);
    echo json_encode($object), ' ', count($object), "\n";
}

echo "-- sort\n";
$ao = new ArrayObject(['b' => 2, 'a' => 1, 'c' => 3]);
$it = $ao->getIterator();
$ao->uasort(function ($x, $y) use ($it) {
    // Written to a copy that the sorted array replaces
    $it['d'] = 4;
    unset($it['c']);

    return $x <=> $y;
});
echo json_encode($ao->getArrayCopy()), "\n";

echo "-- comparison\n";
$a = new ArrayObject();
$a[] = new Writer($a);
$a[] = 'y';
$b = new ArrayObject(['x', 'y']);
var_dump($a == $b, count($a));
?>
--EXPECT--
-- json_encode()
{"a":"written","b":2,"c":3} 23
{"a":"written","b":2,"c":3} 23
-- sort
{"a":1,"b":2,"c":3}
-- comparison
bool(true)
int(3)

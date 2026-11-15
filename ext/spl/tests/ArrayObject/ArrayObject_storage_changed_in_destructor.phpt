--TEST--
ArrayObject: the destructor of a replaced value can change the backing store
--FILE--
<?php
class D {
    public function __construct(private Closure $onDestruct) {}

    public function __destruct()
    {
        ($this->onDestruct)();
    }
}

function grow(ArrayObject $ao): D
{
    return new D(function () use ($ao) {
        // Makes the storage grow, which reallocates it
        for ($i = 0; $i < 100; $i++) {
            $ao["k$i"] = $i;
        }
    });
}

echo "-- the storage grows, string key\n";
$ao = new ArrayObject();
$ao['a'] = grow($ao);
$ao['a'] = 'new';
var_dump(count($ao), $ao['a']);

echo "-- the storage grows, integer key\n";
$ao = new ArrayObject();
$ao[0] = grow($ao);
$ao[0] = 'new';
var_dump(count($ao), $ao[0]);

echo "-- the storage is replaced\n";
$ao = new ArrayObject();
$ao['a'] = new D(function () use ($ao) {
    $ao->exchangeArray(['replaced' => 1]);
});
$ao['a'] = 'new';
var_dump($ao->getArrayCopy());
?>
--EXPECT--
-- the storage grows, string key
int(101)
string(3) "new"
-- the storage grows, integer key
int(101)
string(3) "new"
-- the storage is replaced
array(1) {
  ["replaced"]=>
  int(1)
}

--TEST--
Collections\Deque constructed from Traversable
--FILE--
<?php

function yields_values() {
    for ($i = 0; $i < 10; $i++) {
        yield "r$i" => "s$i";
    }
    $o = (object)['key' => 'value'];
    yield $o => $o;
    yield 0 => 1;
    yield 0 => 2;
    echo "Done evaluating the generator\n";
}

// Collections\Deque eagerly evaluates the passed in Traversable
$it = new Collections\Deque(yields_values());
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
echo "Rewind and iterate again starting from r0\n";
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
unset($it);

$emptyIt = new Collections\Deque(new ArrayObject());
var_dump($emptyIt);
foreach ($emptyIt as $key => $value) {
    echo "Unreachable\n";
}
foreach ($emptyIt as $key => $value) {
    echo "Unreachable\n";
}
echo "Done\n";


?>
--EXPECT--
Done evaluating the generator
Key: 0
Value: 's0'
Key: 1
Value: 's1'
Key: 2
Value: 's2'
Key: 3
Value: 's3'
Key: 4
Value: 's4'
Key: 5
Value: 's5'
Key: 6
Value: 's6'
Key: 7
Value: 's7'
Key: 8
Value: 's8'
Key: 9
Value: 's9'
Key: 10
Value: (object) array(
   'key' => 'value',
)
Key: 11
Value: 1
Key: 12
Value: 2
Rewind and iterate again starting from r0
Key: 0
Value: 's0'
Key: 1
Value: 's1'
Key: 2
Value: 's2'
Key: 3
Value: 's3'
Key: 4
Value: 's4'
Key: 5
Value: 's5'
Key: 6
Value: 's6'
Key: 7
Value: 's7'
Key: 8
Value: 's8'
Key: 9
Value: 's9'
Key: 10
Value: (object) array(
   'key' => 'value',
)
Key: 11
Value: 1
Key: 12
Value: 2
object(Collections\Deque)#1 (0) {
}
Done
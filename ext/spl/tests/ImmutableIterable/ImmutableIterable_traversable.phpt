--TEST--
ImmutableIterable constructed from array
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

// ImmutableIterable eagerly evaluates the passed in Traversable
$it = new ImmutableIterable(yields_values());
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
echo "Rewind and iterate again starting from r0\n";
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
unset($it);

$emptyIt = new ImmutableIterable(new ArrayObject());
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
Key: 'r0'
Value: 's0'
Key: 'r1'
Value: 's1'
Key: 'r2'
Value: 's2'
Key: 'r3'
Value: 's3'
Key: 'r4'
Value: 's4'
Key: 'r5'
Value: 's5'
Key: 'r6'
Value: 's6'
Key: 'r7'
Value: 's7'
Key: 'r8'
Value: 's8'
Key: 'r9'
Value: 's9'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: 0
Value: 1
Key: 0
Value: 2
Rewind and iterate again starting from r0
Key: 'r0'
Value: 's0'
Key: 'r1'
Value: 's1'
Key: 'r2'
Value: 's2'
Key: 'r3'
Value: 's3'
Key: 'r4'
Value: 's4'
Key: 'r5'
Value: 's5'
Key: 'r6'
Value: 's6'
Key: 'r7'
Value: 's7'
Key: 'r8'
Value: 's8'
Key: 'r9'
Value: 's9'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: 0
Value: 1
Key: 0
Value: 2
object(ImmutableIterable)#1 (0) {
}
Done
--TEST--
Collections\Deque modification during foreach
--FILE--
<?php

/** Creates a reference-counted version of a literal string to test reference counting. */
function mut(string $s) {
    $s[0] = $s[0];
    return $s;
}

echo "Test push/unshift\n";
$deque = new Collections\Deque(['a', 'b']);
foreach ($deque as $key => $value) {
    if (strlen($value) === 1) {
        $deque->push("{$value}_");
        $deque->unshift("_$value");
    }
    printf("Key: %s Value: %s\n", var_export($key, true), var_export($value, true));
}
var_dump($deque);
echo "Test shift\n";
foreach ($deque as $key => $value) {
    echo "Shifting $key $value\n";
    var_dump($deque->shift());
}

echo "Test shift out of bounds\n";
$deque = new Collections\Deque([mut('a1'), mut('b1'), mut('c1'), mut('d1')]);
foreach ($deque as $key => $value) {
    var_dump($deque->shift());
    var_dump($deque->shift());
    echo "Saw $key: $value\n";
    // iteration does not stop early, iterator points to just before start of Deque
}
var_dump($deque);

echo "Test iteration behavior\n";
$deque = new Collections\Deque([mut('a1'), mut('a2')]);
$it = $deque->getIterator();
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'value' => $it->current()]), "\n";
$deque->shift();
// invalid, outside the range of the deque
echo json_encode(['valid' => $it->valid(), 'key' => $it->key()]), "\n";
$it->next();
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'value' => $it->current()]), "\n";
$deque->unshift('a', 'b');
unset($deque);
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'value' => $it->current()]), "\n";

?>
--EXPECT--
Test push/unshift
Key: 0 Value: 'a'
Key: 2 Value: 'b'
Key: 4 Value: 'a_'
Key: 5 Value: 'b_'
object(Collections\Deque)#1 (6) {
  [0]=>
  string(2) "_b"
  [1]=>
  string(2) "_a"
  [2]=>
  string(1) "a"
  [3]=>
  string(1) "b"
  [4]=>
  string(2) "a_"
  [5]=>
  string(2) "b_"
}
Test shift
Shifting 0 _b
string(2) "_b"
Shifting 0 _a
string(2) "_a"
Shifting 0 a
string(1) "a"
Shifting 0 b
string(1) "b"
Shifting 0 a_
string(2) "a_"
Shifting 0 b_
string(2) "b_"
Test shift out of bounds
string(2) "a1"
string(2) "b1"
Saw 0: a1
string(2) "c1"
string(2) "d1"
Saw 0: c1
object(Collections\Deque)#2 (0) {
}
Test iteration behavior
{"valid":true,"key":0,"value":"a1"}
{"valid":false,"key":null}
{"valid":true,"key":0,"value":"a2"}
{"valid":true,"key":2,"value":"a2"}

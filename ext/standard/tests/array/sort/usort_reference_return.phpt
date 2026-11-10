--TEST--
Sorting callbacks may return references to local or shared values
--FILE--
<?php
function &compareLocal(int $a, int $b): int {
    $result = $a <=> $b;
    return $result;
}
$items = [3, 1, 2];
usort($items, 'compareLocal');
var_dump($items === [1, 2, 3]);

// Converting the comparison result must not modify the referenced string.
$result = '';
$closure = function &($a, $b) use (&$result) {
    $result = (string) ($a <=> $b);
    return $result;
};
$alias =& $closure(3, 1);
$items = ['three' => 3, 'one' => 1, 'two' => 2];
uasort($items, $closure);
var_dump($items === ['one' => 1, 'two' => 2, 'three' => 3]);
var_dump(is_string($alias));
$alias = 'still alive';
echo $result, "\n";

class ReferenceComparator {
    public int $result = 0;
    public function &compare(int $a, int $b): int {
        $this->result = $a <=> $b;
        return $this->result;
    }
}
$items = [3 => 'a', 1 => 'c', 2 => 'b'];
uksort($items, (new ReferenceComparator())->compare(...));
var_dump($items === [1 => 'c', 2 => 'b', 3 => 'a']);

try {
    $items = [2, 1];
    usort($items, function &($a, $b) {
        throw new Exception('expected');
    });
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
still alive
bool(true)
expected

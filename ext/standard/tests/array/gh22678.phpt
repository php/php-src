--TEST--
GH-22678 (Use-after-free in array_multisort() when the comparator mutates the array)
--FILE--
<?php
class Evil {
    public function __toString(): string {
        // Runs from the SORT_STRING comparator; drop the array being sorted.
        foreach (array_keys($GLOBALS['a']) as $k) {
            unset($GLOBALS['a'][$k]);
        }
        $GLOBALS['a'] = [];
        return "x";
    }
}

$a = [];
for ($i = 0; $i < 10; $i++) {
    $a[] = new Evil();
}
$GLOBALS['a'] = &$a;

echo "freed mid-sort: ";
var_dump(array_multisort($a, SORT_STRING));
unset($GLOBALS['a']);

// Non-packed integer keys reach the mixed-to-packed restructure while the sort
// holds its reference on the array.
$b = [5 => 'c', 3 => 'a', 1 => 'b'];
array_multisort($b, SORT_STRING);
echo "repacked: ", implode(',', $b), "\n";

class Boom {
    public function __toString(): string {
        throw new Exception("boom");
    }
}

$c = [new Boom(), new Boom()];
try {
    array_multisort($c, SORT_STRING);
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
freed mid-sort: bool(true)
repacked: a,b,c
Exception: boom

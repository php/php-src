--TEST--
Rehashing updates iterators at both a hole and the next defined element
--FILE--
<?php
function test(array $values, $firstKey, $secondKey, $newKey, int $newValue): void {
    $outerVisits = [];
    $innerVisits = [];
    $first = true;
    foreach ($values as $outerKey => &$outerValue) {
        $outerVisits[] = "$outerKey=>$outerValue";
        if ($first) {
            $first = false;
            foreach ($values as $innerKey => &$innerValue) {
                $innerVisits[] = "$innerKey=>$innerValue";
                if ($innerValue === 11) {
                    // The outer cursor is at a hole, the inner at the next value.
                    unset($values[$firstKey], $values[$secondKey]);
                    $values[$newKey] = $newValue;
                }
            }
            unset($innerValue);
        }
    }
    unset($outerValue);
    echo 'outer: ', implode(' ', $outerVisits), "\n";
    echo 'inner: ', implode(' ', $innerVisits), "\n";
}

// Adding a string key converts packed storage and compacts its holes.
test([10, 11, 12, 13, 14], 0, 1, 'new', 15);

// Inserting into a full mixed table compacts its holes without growing it.
test(['a' => 10, 'b' => 11, 'c' => 12, 'd' => 13,
      'e' => 14, 'f' => 15, 'g' => 16, 'h' => 17], 'a', 'b', 'i', 18);
?>
--EXPECT--
outer: 0=>10 2=>12 3=>13 4=>14 new=>15
inner: 0=>10 1=>11 2=>12 3=>13 4=>14 new=>15
outer: a=>10 c=>12 d=>13 e=>14 f=>15 g=>16 h=>17 i=>18
inner: a=>10 b=>11 c=>12 d=>13 e=>14 f=>15 g=>16 h=>17 i=>18

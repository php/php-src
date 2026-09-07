--TEST--
Array duplication updates iterators at both a hole and the next defined element
--FILE--
<?php
function test(array $values): void {
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
                    unset($values['a'], $values['b']);
                    $copy = $values;
                    // Trigger copy-on-write duplication, which compacts the holes.
                    $values['i'] = 18;
                }
            }
            unset($innerValue);
        }
    }
    unset($outerValue);
    echo 'outer: ', implode(' ', $outerVisits), "\n";
    echo 'inner: ', implode(' ', $innerVisits), "\n";
}

test(['a' => 10, 'b' => 11, 'c' => 12, 'd' => 13,
      'e' => 14, 'f' => 15, 'g' => 16, 'h' => 17]);
?>
--EXPECT--
outer: a=>10 c=>12 d=>13 e=>14 f=>15 g=>16 h=>17 i=>18
inner: a=>10 b=>11 c=>12 d=>13 e=>14 f=>15 g=>16 h=>17 i=>18

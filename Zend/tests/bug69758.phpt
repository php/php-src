--TEST--
Bug #69758 (Item added to array not being removed by array_pop/shift)
--FILE--
<?php
$tokens     = array();
$conditions = array();
for ($i = 0; $i <= 10; $i++) {
    $tokens[$i] = $conditions;

    // First integer must be less than 8
    // and second must be 8, 9 or 10
    if ($i !== 0 && $i !== 8) {
        continue;
    }

    // Add condition and then pop off straight away.
    // Can also use array_shift() here.
    $conditions[$i] = true;
    $oldCondition = array_pop($conditions);
}

// Conditions should be empty.
var_dump($conditions);
?>
--EXPECT--
array(0) {
}

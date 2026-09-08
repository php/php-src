--TEST--
Packed sorting relocates cursors on both a hole and the next defined element
--FILE--
<?php
foreach (['sort', 'rsort'] as $sort) {
    $values = [10, 11, 12, 13, 14];
    $visited = [];
    foreach ($values as &$outer) {
        foreach ($values as $key => &$inner) {
            $visited[] = "$key=>$inner";
            if ($inner === 11) {
                // The outer cursor is at hole 1; the inner cursor is at value 2.
                unset($values[0], $values[1]);
                $sort($values);
            }
        }
        unset($inner);
        break;
    }
    unset($outer);
    echo "$sort: ", implode(' ', $visited), "\n";
}
?>
--EXPECT--
sort: 0=>10 1=>11 0=>12 1=>13 2=>14
rsort: 0=>10 1=>11 0=>14 1=>13 2=>12

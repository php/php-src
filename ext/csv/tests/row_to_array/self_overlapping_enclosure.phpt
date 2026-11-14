--TEST--
Test Csv\row_to_array(): self-overlapping multibyte enclosures round-trip and stay strict
--EXTENSIONS--
csv
--FILE--
<?php
foreach (['aa' => ['a', 'aa', 'aaa', 'xa', 'ax', 'a,a', "a\na", '', 'aaaa'], '--' => ['y-', '-y', '-', '---', 'y--z']] as $enclosure => $fields) {
    foreach ($fields as $field) {
        $row = Csv\array_to_row([$field, 'z'], ',', $enclosure, "\n");
        $parsed = Csv\row_to_array($row, ',', $enclosure, "\n");
        if ($parsed !== [$field, 'z']) {
            echo "Mismatch for ", json_encode($field), " with ", $enclosure, ": ", json_encode($row), " -> ", json_encode($parsed), \PHP_EOL;
        }
    }
}
/* Data after the closing enclosure is still an error */
try {
    var_dump(Csv\row_to_array("--a--b,c\n", ',', '--', "\n"));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
echo "done", \PHP_EOL;
?>
--EXPECT--
ValueError: Enclosure sequence is not closed
done

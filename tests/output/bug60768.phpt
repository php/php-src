--TEST--
Bug #60768 Output buffer not discarded
--FILE--
<?php

global $storage;

ob_start(function($buffer) use (&$storage) { $storage .= $buffer; }, 20);

echo str_repeat("0", 20); // fill in the buffer

for($i = 0; $i < 10; $i++) {
    echo str_pad($i, 9, ' ', STR_PAD_LEFT) . "\n"; // full buffer dumped every time
}

ob_end_flush();

printf("Output size: %d, expected %d\n", strlen($storage), 20 + 10 * 10);

?>
DONE
--EXPECT--
Output size: 120, expected 120
DONE

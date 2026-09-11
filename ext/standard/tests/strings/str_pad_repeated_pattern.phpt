--TEST--
str_pad() with repeated padding patterns and partial final copies
--FILE--
<?php
foreach (['ab', 'abc', "\0\xffab", str_repeat('abc', 11)] as $padding) {
    foreach (['', 'input'] as $input) {
        foreach ([0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17, 31, 32, 33,
                  63, 64, 65, 255, 256, 257, 1023, 1024, 1025] as $extra) {
            $repeated = str_repeat($padding, intdiv($extra, strlen($padding)) + 1);
            $full = substr($repeated, 0, $extra);
            $left = intdiv($extra, 2);
            $expected = [
                STR_PAD_LEFT => $full . $input,
                STR_PAD_RIGHT => $input . $full,
                STR_PAD_BOTH => substr($repeated, 0, $left) . $input
                    . substr($repeated, 0, $extra - $left),
            ];
            foreach ($expected as $type => $value) {
                if (str_pad($input, strlen($input) + $extra, $padding, $type) !== $value) {
                    throw new Exception("Incorrect padding: type=$type, extra=$extra");
                }
            }
        }
    }
}
echo "Done\n";
?>
--EXPECT--
Done

--TEST--
Verifying that the str_increment() polyfill behaves the same
--FILE--
<?php

function polyfill(string $s): string {
    if (is_numeric($s)) {
        $offset = stripos($s, 'e');
        if ($offset !== false) {
            /* Using increment operator would cast the string to float
             * Therefore we manually increment it to convert it to an "f"/"F" that doesn't get affected */
            $c = $s[$offset];
            $c++;
            $s[$offset] = $c;
            $s++;
            $s[$offset] = match ($s[$offset]) {
                'f' => 'e',
                'F' => 'E',
                'g' => 'f',
                'G' => 'F',
            };
            return $s;
        }
    }
    return ++$s;
}

$strictlyAlphaNumeric = [
    "Az",
    "aZ",
    "A9",
    "a9",
    // Carrying values until the beginning of the string
    "Zz",
    "zZ",
    "9z",
    "9Z",
    // string interpretable as a number in scientific notation
    "5e6",
    "5E6",
    "5e9",
    "5E9",
    // Interned strings
    "d",
    "D",
    "4",
];

foreach ($strictlyAlphaNumeric as $s) {
    if (str_increment($s) !== polyfill($s)) {
        var_dump("Error:", str_increment($s), polyfill($s));
    }
}

echo "DONE";

?>
--EXPECT--
DONE

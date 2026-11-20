--TEST--
Test Csv\array_to_row(): fields are enclosed when a token would occur across the field boundary
--EXTENSIONS--
csv
--FILE--
<?php
function round_trip(array $rows, string $delimiter, string $enclosure, string $eol): void {
    $buffer = Csv\collection_to_buffer($rows, $delimiter, $enclosure, $eol);
    echo json_encode($buffer), ' ';
    var_dump(Csv\buffer_to_collection($buffer, $delimiter, $enclosure, $eol) === $rows);
}

/* The field ends with the start of the delimiter following it */
round_trip([['-', 'z']], '--', 'aa', "\r\n");
round_trip([['z-', 'a']], '--', 'aa', "\r\n");
/* The field ends with the start of the EOL sequence following it */
round_trip([['a', 'b|']], ',', '"', '||');
/* The field starts with the end of the enclosure, which begins with the delimiter before it */
round_trip([['a', 'x'], ['b', 'xy']], '-', '-x', "\n");
/* An empty field after the EOL sequence would make it read as the delimiter starting with it */
round_trip([['a', ''], ['', 'b']], '||', '"', '|');
/* With the usual single byte tokens, only fields containing a token are enclosed */
round_trip([['a', 'b,c', 'd"e', "f\ng"]], ',', '"', "\r\n");
?>
--EXPECT--
"aa-aa--z\r\n" bool(true)
"aaz-aa--a\r\n" bool(true)
"a,\"b|\"||" bool(true)
"a--xx-x\nb--xxy-x\n" bool(true)
"a|||\"\"||b|" bool(true)
"a,\"b,c\",\"d\"\"e\",\"f\ng\"\r\n" bool(true)

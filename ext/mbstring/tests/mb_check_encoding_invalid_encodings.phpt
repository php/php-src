--TEST--
mb_check_encoding() with invalid encodings
--EXTENSIONS--
mbstring
--FILE--
<?php

$str = "Normal string";
$arr = [1234, 12.34, TRUE, FALSE, NULL, $str, 'key'=>$str, $str=>'val'];

echo 'Using "BAD" as encoding' . \PHP_EOL;
try {
    var_dump(mb_check_encoding($str, 'BAD'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_check_encoding($arr, 'BAD'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo 'Using "pass" as encoding' . \PHP_EOL;
try {
    var_dump(mb_check_encoding($str, 'pass'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_check_encoding($arr, 'pass'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Using "BAD" as encoding
mb_check_encoding(): Argument #2 ($encoding) must be a valid encoding, "BAD" given
mb_check_encoding(): Argument #2 ($encoding) must be a valid encoding, "BAD" given
Using "pass" as encoding
mb_check_encoding(): Argument #2 ($encoding) must be a valid encoding, "pass" given
mb_check_encoding(): Argument #2 ($encoding) must be a valid encoding, "pass" given

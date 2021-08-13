--TEST--
mb_str*() - unknown encoding
--CREDITS--
Jachim Coudenys <jachimcoudenys@gmail.com>
User Group: PHP-WVL & PHPGent #PHPTestFest
--EXTENSIONS--
mbstring
--FILE--
<?php

try {
    mb_chr(1, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_convert_case('coudenys', MB_CASE_UPPER, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_convert_encoding('coudenys', 'UTF-8', 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_convert_kana('coudenys', 'KV', 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_decode_numericentity('coudenys', [], 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_ord('coudenys', 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strcut('coudenys', 0, 4, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strimwidth('coudenys', 0, 4, '', 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_stripos('coudenys', 'cou', 0, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_stristr('coudenys', 'cou', false, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strlen('coudenys', 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strpos('coudenys', 'cou', 0, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strrchr('coudenys', 'cou', false, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strrichr('coudenys', 'cou', false, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strripos('coudenys', 'cou', 0, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strrpos('coudenys', 'cou', 0, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strstr('coudenys', 'cou', false, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strtolower('coudenys', 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strtoupper('coudenys', 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_strwidth('coudenys', 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_substr('coudenys', 0, null, 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    mb_substr_count('coudenys', 'c', 'UTF-0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
mb_chr(): Argument #2 ($encoding) must be a valid encoding, "UTF-0" given
mb_convert_case(): Argument #3 ($encoding) must be a valid encoding, "UTF-0" given
mb_convert_encoding(): Argument #3 ($from_encoding) contains invalid encoding "UTF-0"
mb_convert_kana(): Argument #3 ($encoding) must be a valid encoding, "UTF-0" given
mb_decode_numericentity(): Argument #3 ($encoding) must be a valid encoding, "UTF-0" given
mb_ord(): Argument #2 ($encoding) must be a valid encoding, "UTF-0" given
mb_strcut(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_strimwidth(): Argument #5 ($encoding) must be a valid encoding, "UTF-0" given
mb_stripos(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_stristr(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_strlen(): Argument #2 ($encoding) must be a valid encoding, "UTF-0" given
mb_strpos(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_strrchr(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_strrichr(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_strripos(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_strrpos(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_strstr(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_strtolower(): Argument #2 ($encoding) must be a valid encoding, "UTF-0" given
mb_strtoupper(): Argument #2 ($encoding) must be a valid encoding, "UTF-0" given
mb_strwidth(): Argument #2 ($encoding) must be a valid encoding, "UTF-0" given
mb_substr(): Argument #4 ($encoding) must be a valid encoding, "UTF-0" given
mb_substr_count(): Argument #3 ($encoding) must be a valid encoding, "UTF-0" given

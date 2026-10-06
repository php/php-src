--TEST--
strncmp()/strncasecmp() $length must not be narrowed to size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
/* Both functions hand $length to zend_binary_strncmp()/zend_binary_strncasecmp()
 * as a size_t. Where zend_long is wider, a length above SIZE_MAX is narrowed:
 * SIZE_MAX + 1 becomes 0 and SIZE_MAX + 3 becomes 2, so the comparison stops
 * short and reports equality for operands that differ further along. Any length
 * at or beyond the operand lengths has to behave like SIZE_MAX, which is what
 * the comparators already do internally with MIN(length, MIN(len1, len2)). */
$sizeMax = 2 ** (PHP_SYS_SIZE * 8) - 1;

$lengths = [
    '2'          => 2,
    '3'          => 3,
    'SIZE_MAX'   => $sizeMax,
    'SIZE_MAX+1' => $sizeMax + 1,
    'SIZE_MAX+3' => $sizeMax + 3,
    'INT_MAX'    => PHP_INT_MAX,
];

foreach ($lengths as $label => $length) {
    echo "$label: ";
    printf("strncmp=%d strncasecmp=%d\n",
        strncmp('abc', 'abd', $length),
        strncasecmp('ABC', 'abd', $length));
}
?>
--EXPECT--
2: strncmp=0 strncasecmp=0
3: strncmp=-1 strncasecmp=-1
SIZE_MAX: strncmp=-1 strncasecmp=-1
SIZE_MAX+1: strncmp=-1 strncasecmp=-1
SIZE_MAX+3: strncmp=-1 strncasecmp=-1
INT_MAX: strncmp=-1 strncasecmp=-1

--TEST--
convert_uudecode() with invalid data
--FILE--
<?php
$warning = null;
set_error_handler(function (int $errno, string $errstr) use (&$warning) {
    $warning = $errstr;
    return true;
});

$encoded100 = convert_uuencode(str_repeat('a', 100));
$invalid = [
    'empty string'                         => '',
    'length character only'                => '!',
    'truncated group'                      => '!80',
    'full line announced, no data'         => 'M',
    'maximum line announced, no data'      => '_',
    'full line announced, short data'      => 'M' . str_repeat('abcd', 5),
    'plain text'                           => 'not uuencoded',
    'multi-line, cut inside first line'    => substr($encoded100, 0, 30),
    'multi-line, cut inside second line'   => substr($encoded100, 0, 92),
    'multi-line, cut inside last line'     => substr($encoded100, 0, 132),
];
foreach ($invalid as $label => $input) {
    $warning = 'no warning';
    $result = convert_uudecode($input);
    echo $label, ": ", var_export($result, true), ", ", $warning, "\n";
}
?>
--EXPECT--
empty string: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string
length character only: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string
truncated group: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string
full line announced, no data: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string
maximum line announced, no data: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string
full line announced, short data: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string
plain text: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string
multi-line, cut inside first line: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string
multi-line, cut inside second line: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string
multi-line, cut inside last line: false, convert_uudecode(): Argument #1 ($string) is not a valid uuencoded string

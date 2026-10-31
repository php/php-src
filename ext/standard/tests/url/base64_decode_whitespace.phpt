--TEST--
base64_decode() with whitespace, padding and invalid bytes inside the input, in both modes
--FILE--
<?php
$clean = base64_encode("The quick brown fox jumps over the lazy dog, 0123456789.");

$inputs = [
    'clean' => $clean,
    'leading and trailing whitespace' => " \t\r\n" . $clean . "\n\n",
    'space inside a group' => substr($clean, 0, 2) . ' ' . substr($clean, 2),
    'space between groups' => substr($clean, 0, 4) . ' ' . substr($clean, 4),
    'a space after every character' => implode(' ', str_split($clean)),
    'MIME lines' => chunk_split($clean, 76, "\r\n"),
    'PEM lines' => chunk_split($clean, 64, "\n"),
    'short lines' => chunk_split($clean, 7, "\n"),
    'line break before the padding' => substr($clean, 0, -2) . "\n" . substr($clean, -2),
    'line break inside the padding' => substr($clean, 0, -1) . "\n" . substr($clean, -1),
    'line break after the padding' => $clean . "\n",
    'no padding' => rtrim($clean, '='),
    'no padding, trailing whitespace' => rtrim($clean, '=') . " \n",
    'data after the padding' => $clean . 'QUJD',
    'padding in the middle' => substr($clean, 0, 8) . '==' . substr($clean, 8),
    'invalid byte inside a group' => substr($clean, 0, 2) . '*' . substr($clean, 2),
    'invalid byte between groups' => substr($clean, 0, 4) . '-' . substr($clean, 4),
    'high byte' => substr($clean, 0, 4) . "\xC3\xA9" . substr($clean, 4),
    'one character' => 'Q',
    'two characters' => 'QU',
    'three characters' => 'QUJ',
    'five characters' => 'QUJDR',
    'only whitespace' => " \n\t",
    'only padding' => '====',
    'empty' => '',
];

function show($result) {
    if ($result === false) {
        return 'false';
    }
    $plain = ' .,' . implode('', range('0', '9')) . implode('', range('a', 'z')) . implode('', range('A', 'Z'));
    return strspn($result, $plain) === strlen($result) ? "'$result'" : 'bytes ' . bin2hex($result);
}

foreach ($inputs as $name => $input) {
    printf("%-34s %s | %s\n", $name, show(base64_decode($input)), show(base64_decode($input, true)));
}

// every length around the group and vector block boundaries, with a line break at every position
$ok = true;
for ($bytes = 0; $bytes <= 100; $bytes++) {
    $raw = $bytes ? random_bytes($bytes) : "";
    $encoded = base64_encode($raw);
    for ($at = 0; $at <= strlen($encoded); $at++) {
        $broken = substr($encoded, 0, $at) . "\r\n" . substr($encoded, $at);
        if (base64_decode($broken) !== $raw || base64_decode($broken, true) !== $raw) {
            echo "mismatch: $bytes bytes, line break at $at\n";
            $ok = false;
        }
    }
    $mime = chunk_split($encoded, 76, "\r\n");
    if (base64_decode($mime) !== $raw || base64_decode($mime, true) !== $raw) {
        echo "mismatch: $bytes bytes, MIME\n";
        $ok = false;
    }
}
var_dump($ok);
?>
--EXPECT--
clean                              'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
leading and trailing whitespace    'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
space inside a group               'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
space between groups               'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
a space after every character      'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
MIME lines                         'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
PEM lines                          'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
short lines                        'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
line break before the padding      'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
line break inside the padding      'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
line break after the padding       'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
no padding                         'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
no padding, trailing whitespace    'The quick brown fox jumps over the lazy dog, 0123456789.' | 'The quick brown fox jumps over the lazy dog, 0123456789.'
data after the padding             bytes 54686520717569636b2062726f776e20666f78206a756d7073206f76657220746865206c617a7920646f672c20303132333435363738392e105090 | false
padding in the middle              'The quick brown fox jumps over the lazy dog, 0123456789.' | false
invalid byte inside a group        'The quick brown fox jumps over the lazy dog, 0123456789.' | false
invalid byte between groups        'The quick brown fox jumps over the lazy dog, 0123456789.' | false
high byte                          'The quick brown fox jumps over the lazy dog, 0123456789.' | false
one character                      '' | false
two characters                     'A' | 'A'
three characters                   'AB' | 'AB'
five characters                    'ABC' | false
only whitespace                    '' | ''
only padding                       '' | false
empty                              '' | ''
bool(true)

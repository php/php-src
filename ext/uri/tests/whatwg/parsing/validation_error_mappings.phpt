--TEST--
Test Uri\WhatWg\Url validation error mappings
--FILE--
<?php

$inputs = [
    'http://1.2.3./',
    'http://1.a.2.3/',
    'http://256.0.0.1/',
    'http://0x1.2.3.4/',
    'http://[::1',
    'http://[:1]/',
    'http://[1:2:3:4:5:6:7:8:9]/',
    'http://[1::1::1]/',
    'http://[::g]/',
    'http://[1:2:3]/',
    'http://[1:2:3:4:5:6:7:1.2.3.4]/',
    'http://[::ffff:1.2.3.a]/',
    'http://[::ffff:256.2.3.4]/',
    'http://[::ffff:1.2.3]/',
    'http://example.com\foo',
    'file://c:/foo',
];

foreach ($inputs as $input) {

    $errors = [];
    $url = Uri\WhatWg\Url::parse($input, errors: $errors);

    echo $input, "\n";
    var_dump($url?->toAsciiString());

    foreach ($errors as $error) {
        echo $error->type->name, ': ', $error->context, ': ';
        var_dump($error->failure);
    }
}

$errors = [];
$url = Uri\WhatWg\Url::parse('c|/foo', new Uri\WhatWg\Url('file:///tmp/base'), $errors);

echo 'c|/foo with file base', "\n";
var_dump($url?->toAsciiString());

foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
http://1.2.3./
string(15) "http://1.2.0.3/"
Ipv4EmptyPart: : bool(false)
http://1.a.2.3/
NULL
Ipv4NonNumericPart: a.2.3: bool(true)
http://256.0.0.1/
NULL
Ipv4OutOfRangePart: 256.0.0.1: bool(true)
http://0x1.2.3.4/
string(15) "http://1.2.3.4/"
Ipv4NonDecimalPart: 0x1.2.3.4: bool(false)
http://[::1
NULL
Ipv6Unclosed: 1: bool(true)
http://[:1]/
NULL
Ipv6InvalidCompression: 1]/: bool(true)
http://[1:2:3:4:5:6:7:8:9]/
NULL
Ipv6TooManyPieces: 9]/: bool(true)
http://[1::1::1]/
NULL
Ipv6MultipleCompression: :1]/: bool(true)
http://[::g]/
NULL
Ipv6InvalidCodePoint: g]/: bool(true)
http://[1:2:3]/
NULL
Ipv6TooFewPieces: ]/: bool(true)
http://[1:2:3:4:5:6:7:1.2.3.4]/
NULL
Ipv4InIpv6TooManyPieces: 1.2.3.4]/: bool(true)
http://[::ffff:1.2.3.a]/
NULL
Ipv4InIpv6InvalidCodePoint: a]/: bool(true)
http://[::ffff:256.2.3.4]/
NULL
Ipv4InIpv6OutOfRangePart: 6.2.3.4]/: bool(true)
http://[::ffff:1.2.3]/
NULL
Ipv4InIpv6TooFewParts: ]/: bool(true)
http://example.com\foo
string(22) "http://example.com/foo"
InvalidReverseSoldius: \foo: bool(false)
file://c:/foo
string(14) "file:///c:/foo"
FileInvalidWindowsDriveLetterHost: c:/foo: bool(false)
c|/foo with file base
string(14) "file:///c:/foo"
InvalidUrlUnit: |/foo: bool(false)
FileInvalidWindowsDriveLetter: c|/foo: bool(false)

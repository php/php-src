--TEST--
Case-sensitivity part of bug #64874 ("json_decode handles whitespace and case-sensitivity incorrectly")
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
function decode($json) {
    var_dump(json_decode($json));
    var_dump(json_last_error() !== 0);
    echo "\n";
}

// Only lowercase should work
decode('true');
decode('True');
decode('false');
decode('False');
decode('null');
decode('Null');

echo "Done\n";
--EXPECT--
bool(true)
bool(false)

NULL
bool(true)

bool(false)
bool(false)

NULL
bool(true)

NULL
bool(false)

NULL
bool(true)

Done

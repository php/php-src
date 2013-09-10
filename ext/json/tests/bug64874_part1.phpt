--TEST--
Whitespace part of bug #64874 ("json_decode handles whitespace and case-sensitivity incorrectly")
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
function decode($json) {
    var_dump(json_decode($json));
    var_dump(json_last_error() !== 0);
    echo "\n";
}

// Leading whitespace should be ignored
decode(" true");
decode("\ttrue");
decode("\ntrue");
decode("\rtrue");

// So should trailing whitespace
decode("true ");
decode("true\t");
decode("true\n");
decode("true\r");

echo "Done\n";
--EXPECT--
bool(true)
bool(false)

bool(true)
bool(false)

bool(true)
bool(false)

bool(true)
bool(false)

bool(true)
bool(false)

bool(true)
bool(false)

bool(true)
bool(false)

bool(true)
bool(false)

Done

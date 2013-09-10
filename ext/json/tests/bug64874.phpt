--TEST--
Bug #64874 (json_decode does not properly decode with options parameter)
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

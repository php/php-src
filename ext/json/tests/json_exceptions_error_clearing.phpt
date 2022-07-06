--TEST--
JSON_THROW_ON_ERROR: global error flag untouched
--FILE--
<?php

var_dump(json_last_error());

// here we cause a different kind of error to the following errors, so that
// we can be sure the global error state looking unchanged isn't coincidence
json_decode("\xFF");

var_dump(json_last_error());

try {
    json_decode("", false, 512, JSON_THROW_ON_ERROR);
} catch (JsonException $e) {
    echo "Caught JSON exception: ", $e->getCode(), PHP_EOL;
}

var_dump(json_last_error());

try {
    json_decode("{", false, 512, JSON_THROW_ON_ERROR);
} catch (JsonException $e) {
    echo "Caught JSON exception: ", $e->getCode(), PHP_EOL;
}

var_dump(json_last_error());


try {
    json_encode(NAN, JSON_THROW_ON_ERROR);
} catch (JsonException $e) {
    echo "Caught JSON exception: ", $e->getCode(), PHP_EOL;
}

var_dump(json_last_error());

?>
--EXPECT--
int(0)
int(5)
Caught JSON exception: 4
int(5)
Caught JSON exception: 4
int(5)
Caught JSON exception: 7
int(5)

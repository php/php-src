--TEST--
curl_write(): edge cases for write callback return values
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start('echo "Hello World!";');

$values = [
    'valid length' => 'valid',
    'null' => null,
    'false' => false,
    'true' => true,
    'zero' => 0,
    'one' => 1,
    'negative one' => -1,
    'PHP_INT_MAX' => PHP_INT_MAX,
    'PHP_INT_MIN' => PHP_INT_MIN,
    'float' => 1.5,
    'negative float' => -1.5,
    'numeric string' => '25',
    'empty string' => '',
    'non-numeric string' => 'abc',
    'large numeric string' => '999999999999999999999999999999999999',
    'empty array' => [],
    'array' => [1],
    'object' => new stdClass(),
];

foreach ($values as $label => $value) {
    echo "-- $label --\n";

    $ch = curl_init($host);
    curl_setopt($ch, CURLOPT_WRITEFUNCTION,
        static function ($ch, $data) use ($value) {
            return $value === 'valid' ? strlen($data) : $value;
        }
    );

    try {
        var_dump(curl_exec($ch));
    } catch (Throwable $e) {
        echo get_class($e), "\n";
    }

}
?>
--EXPECTF--
-- valid length --
bool(true)
-- null --
bool(false)
-- false --
bool(false)
-- true --
bool(false)
-- zero --
bool(false)
-- one --
bool(false)
-- negative one --
bool(false)
-- PHP_INT_MAX --
bool(false)
-- PHP_INT_MIN --
bool(false)
-- float --

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line 38
bool(false)
-- negative float --

Deprecated: Implicit conversion from float -1.5 to int loses precision in %s on line 38
bool(false)
-- numeric string --
bool(true)
-- empty string --
bool(false)
-- non-numeric string --
bool(false)
-- large numeric string --

Deprecated: Implicit conversion from float-string "999999999999999999999999999999999999" to int loses precision in %s on line 38
bool(false)
-- empty array --
bool(false)
-- array --
bool(false)
-- object --
bool(false)

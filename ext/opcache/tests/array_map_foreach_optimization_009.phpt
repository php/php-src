--TEST--
array_map(): foreach optimization - dynamic-call drift bug
--CREDITS--
Ryan @ Calif.io
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

function cand_86013_trusted(string $value): string
{
    return 'trusted:' . $value;
}

function cand_86013_unexpected(string $value): string
{
    return 'unexpected:' . $value;
}

function cand_86013_input_changes_target(): array
{
    global $callback;
    global $obj;
    $callback = 'cand_86013_unexpected';
    $obj = new Unexpected;
    return ['payload'];
}

class Trusted {
    static function f($value) {
        return 'trusted:' . $value;
    }
}

class Unexpected {
    static function f($value) {
        return 'unexpected:' . $value;
    }
}

$callback = 'cand_86013_trusted';
echo "direct array_map\n";
var_dump(array_map($callback(...), cand_86013_input_changes_target()));

$callback = 'cand_86013_trusted';
$array_map = 'array_map';
echo "dynamic-call control\n";
var_dump($array_map($callback(...), cand_86013_input_changes_target()));

$missing = 'cand_86013_missing';
echo "empty direct\n";
try {
    var_dump(array_map($missing(...), []));
} catch (Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

echo "empty dynamic-call control\n";
try {
    var_dump($array_map($missing(...), []));
} catch (Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

$obj = new Trusted;
echo "direct array_map static call\n";
var_dump(array_map($obj::f(...), cand_86013_input_changes_target()));

?>
--EXPECT--
direct array_map
array(1) {
  [0]=>
  string(15) "trusted:payload"
}
dynamic-call control
array(1) {
  [0]=>
  string(15) "trusted:payload"
}
empty direct
array(0) {
}
empty dynamic-call control
Error: Call to undefined function cand_86013_missing()
direct array_map static call
array(1) {
  [0]=>
  string(15) "trusted:payload"
}

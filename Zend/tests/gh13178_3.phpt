--TEST--
GH-13178: Unsetting last offset variation with hashed array
--FILE--
<?php

$data = ['foo' => 'foo', 'bar' => 'bar', 'baz' => 'baz'];

foreach ($data as $key => &$value) {
    var_dump($value);
    if ($value === 'baz') {
        unset($data['bar']);
        unset($data['baz']);
        $data['qux'] = 'qux';
        $data['quux'] = 'quux';
    }
}

?>
--EXPECT--
string(3) "foo"
string(3) "bar"
string(3) "baz"
string(3) "qux"
string(4) "quux"

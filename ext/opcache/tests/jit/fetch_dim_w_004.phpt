--TEST--
JIT FETCH_DIM_W: 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function create_references(&$array) {
    foreach ($a as $key => $value) {
        create_references($array[$key]);
    }
}

function change_copy($copy) {
        $copy['b']['z']['z'] = $copy['b'];
}

$data = [
    'a' => [
        'b' => [],
    ],
];

@create_references($data);

$copy = $data['a'];
var_dump($copy);

change_copy($copy);
var_dump($copy); //RECURSION
?>
--EXPECT--
array(1) {
  ["b"]=>
  array(0) {
  }
}
array(1) {
  ["b"]=>
  array(0) {
  }
}

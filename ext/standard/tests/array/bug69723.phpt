--TEST--
Bug #69723 (Passing parameters by reference and array_column)
--FILE--
<?php
function byReference( & $array){
    foreach($array as &$item){
        $item['nanana'] = 'batman';
        $item['superhero'] = 'robin';
    }
}

$array = [
    [
    'superhero'=> 'superman',
    'nanana' => 'no nana'
    ],
    [
    'superhero'=> 'acuaman',
    'nanana' => 'no nana'
    ],

    ];

var_dump(array_column($array, 'superhero'));
byReference($array);
var_dump(array_column($array, 'superhero'));
?>
--EXPECT--
array(2) {
  [0]=>
  string(8) "superman"
  [1]=>
  string(7) "acuaman"
}
array(2) {
  [0]=>
  string(5) "robin"
  [1]=>
  string(5) "robin"
}

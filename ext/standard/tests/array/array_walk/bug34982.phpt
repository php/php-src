--TEST--
Bug #34982 (array_walk_recursive() modifies elements outside function scope)
--FILE--
<?php
$ar = array(
    'element 1',
    array('subelement1')
    );

func($ar);
print_r($ar);

function func($a) {
  array_walk_recursive($a, 'apply');
  print_r($a);
}

function apply(&$input, $key) {
  $input = 'changed';
}
?>
--EXPECT--
Array
(
    [0] => changed
    [1] => Array
        (
            [0] => changed
        )

)
Array
(
    [0] => element 1
    [1] => Array
        (
            [0] => subelement1
        )

)

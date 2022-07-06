--TEST--
Bug #78502: Incorrect stack size calculation for indirectly recursive function call
--FILE--
<?php

$tree = [
    'name' => 'a',
    'quant' => 1,
    'children' => [
        ['name' => 'b', 'quant' => 1],
        ['name' => 'c', 'quant' => 1, 'children' => [
            ['name' => 'd', 'quant' => 1],
        ]],
    ],
];

function tree_map($tree, $recursive_attr, closure $callback){
    if(isset($tree[$recursive_attr])){
        $tree[$recursive_attr] = array_map(function($c) use($recursive_attr, $callback){
            return tree_map($c, $recursive_attr, $callback);
        }, $tree[$recursive_attr]);
    }
    return $callback($tree);
}

tree_map($tree, 'children', function ($node) {});

?>
===DONE===
--EXPECT--
===DONE===

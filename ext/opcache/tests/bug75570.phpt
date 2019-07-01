--TEST--
Bug #75570 ("Narrowing occurred during type inference" error)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function renderRawGraph(array $parents) {
    $graph = array();
    foreach ($parents as $p) {
        foreach ($graph as $v) {
            $graph[$v]['line'] = 1;
        }
        $graph[] = array(
            'line' => 1,
        );
    }
}
?>
OK
--EXPECT--
OK

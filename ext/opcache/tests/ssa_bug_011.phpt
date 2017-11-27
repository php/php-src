--TEST--
Type inference warning
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

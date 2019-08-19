--TEST--
Bug #70895 null ptr deref and segfault with crafted callable
--FILE--
<?php
function m($f,$a){
    return array_map($f,0);
}

try {
    echo implode(m("",m("",m("",m("",m("0000000000000000000000000000000000",("")))))));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
array_map() expects parameter 1 to be a valid callback, function '0000000000000000000000000000000000' not found or invalid function name

--TEST--
GH-20727 (User code can run after module request shutdown via the output layer)
--FILE--
<?php
putenv('foo=baz');
header_register_callback(function(){
    var_dump(putenv('foo=bar'));
});
?>
--EXPECT--
bool(true)

--TEST--
Bailout (E_ERROR) during UserStream Open
--EXTENSIONS--
zend_test
--FILE--
<?php

class FailStream {
    public $context;
    public function stream_open($path, $mode, $options, &$opened_path) {
        zend_trigger_bailout();
    }
}
stream_wrapper_register('mystream', 'FailStream');
fopen('mystream://foo', 'r');
echo 'Done';
?>
--EXPECTF--
Fatal error: Bailout in %s on line %d

--TEST--
Bailout (E_ERROR) during UserStream Dir Open
--EXTENSIONS--
zend_test
--FILE--
<?php

class FailStream {
    public $context;
    public function dir_opendir(string $url, int $options) {
        zend_trigger_bailout();
    }
}
stream_wrapper_register('mystream', 'FailStream');
opendir('mystream://foo');
echo 'Done';
?>
--EXPECTF--
Fatal error: Bailout in %s on line %d

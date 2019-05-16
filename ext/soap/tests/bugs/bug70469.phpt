--TEST--
Bug #70469 (SoapClient should not generate E_ERROR if exceptions enabled)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
try {
    $x = new SoapClient('http://i_dont_exist.com/some.wsdl');
} catch (SoapFault $e) {
    echo "catched\n";
}

$error = error_get_last();
if ($error === null) {
    echo "ok\n";
}
?>
--EXPECT--
catched
ok

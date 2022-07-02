--TEST--
Bug #70469 (SoapClient should not generate E_ERROR if exceptions enabled)
--EXTENSIONS--
soap
--FILE--
<?php
try {
    $x = new SoapClient('http://i_dont_exist.com/some.wsdl');
} catch (SoapFault $e) {
    echo "caught\n";
}

$error = error_get_last();
if ($error === null) {
    echo "ok\n";
}
?>
--EXPECT--
caught
ok

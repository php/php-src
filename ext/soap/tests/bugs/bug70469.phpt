--TEST--
Bug #70469 (SoapClient should not generate E_ERROR if exceptions enabled)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
set_error_handler(function($a,$b,$c,$d) { throw new ErrorException($b,0,$a,$c,$d); }, -1);

try {
    new SoapClient('http://i_dont_exist.com/some.wsdl');
}
catch (SoapFault $e) {
	echo "SoapFault\n";
}
catch (ErrorException $e) {
	echo "ErrorException\n";
}
catch (Exception $e) {
	echo "Exception\n";
}
echo "I survived\n";
register_shutdown_function(function() {
	echo 'Shutdown';
});
?>
--EXPECT--
SoapFault
I survived
Shutdown

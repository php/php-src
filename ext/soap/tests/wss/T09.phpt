--TEST--
SOAP CLIENT WSS: Type and value error handling
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
include __DIR__.DIRECTORY_SEPARATOR."wss-test.inc";

$client = new LocalSoapClient(__DIR__.DIRECTORY_SEPARATOR."wss-test.wsdl");

try { 
	$client->__setWSS(false);
} catch (TypeError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc" => "__fail_me"
	));
} catch (TypeError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken"	=> false
	));
} catch (TypeError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken"	=> null,
		"digest_method"		=> false,
	));
} catch (TypeError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken"	=> null,
		"digest_method"		=> 999,
	));
} catch (ValueError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken"	=> null,
		"digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA256,
		"add_timestamp"		=> null
	));
} catch (TypeError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken"	=> null,
		"digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA256,
		"add_timestamp"		=> true,
		"timestamp_expires"	=> null
	));
} catch (TypeError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken"	=> null,
		"digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA256,
		"add_timestamp"		=> true,
		"timestamp_expires"	=> -1
	));
} catch (ValueError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken"	=> null,
		"digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA256,
		"add_timestamp"		=> true,
		"timestamp_expires"	=> 120,
		"random_id"			=> null
	));
} catch (TypeError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken"	=> null,
		"digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA256,
		"add_timestamp"		=> true,
		"timestamp_expires"	=> 120,
		"random_id"			=> true
	));
} catch (TypeError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken" 	=> null,
		"digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA256,
		"add_timestamp"		=> true,
		"timestamp_expires"	=> 120,
		"random_id"			=> true,
		"wss_version"		=> null
	));
} catch (TypeError $e) { 
	var_dump($e->getMessage());
}

try { 
	$client->__setWSS(array(
		"signfunc"			=> function($data) {
			return $data;
		},
		"x509_binsectoken"	=> null,
		"digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA256,
		"add_timestamp"		=> true,
		"timestamp_expires"	=> 120,
		"random_id"			=> true,
		"wss_version"		=> 999
	));
} catch (ValueError $e) { 
	var_dump($e->getMessage());
}

var_dump($client->__setWSS(array(
	"signfunc"			=> function($data) {
		return $data;
	},
	"x509_binsectoken"	=> null,
	"digest_method"		=> SOAP_WSS_DIGEST_METHOD_SHA256,
	"add_timestamp"		=> true,
	"timestamp_expires"	=> 120,
	"random_id"			=> true,
	"wss_version"		=> SOAP_WSS_VERSION_1_1
)));
?>
--EXPECTF--
string(81) "SoapClient::__setWSS(): Argument #1 ($options) must be of type ?array, bool given"
string(112) "__setWSS(): "signfunc" must be a valid callback or null, function "__fail_me" not found or invalid function name"
string(73) "__setWSS(): "x509_binsectoken" must be of type string or null, bool given"
string(59) "__setWSS(): "digest_method" must be of type int, bool given"
string(52) "__setWSS(): invalid "digest_method" value, 999 given"
string(60) "__setWSS(): "add_timestamp" must be of type bool, null given"
string(63) "__setWSS(): "timestamp_expires" must be of type int, null given"
string(55) "__setWSS(): invalid "timestamp_expires" value, -1 given"
string(56) "__setWSS(): "random_id" must be of type bool, null given"
string(57) "__setWSS(): "wss_version" must be of type int, null given"
string(50) "__setWSS(): invalid "wss_version" value, 999 given"
bool(true)

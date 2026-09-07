--TEST--
GH-21421 (SoapClient typemap property breaks engine assumptions)
--EXTENSIONS--
soap
--FILE--
<?php
$options = array(
'uri'      => 'http://schemas.nothing.com',
'location' => 'test://',
'typemap'  => array(array("type_ns"   => "http://schemas.nothing.com",
"type_name" => "book",
"from_xml"  => "book_from_xml"))
);
$client = new SoapClient(NULL, $options);
var_dump($client);
?>
--EXPECTF--
object(SoapClient)#%d (35) {
  ["uri":"SoapClient":private]=>
  string(26) "http://schemas.nothing.com"
  ["style":"SoapClient":private]=>
  NULL
  ["use":"SoapClient":private]=>
  NULL
  ["location":"SoapClient":private]=>
  string(7) "test://"
  ["trace":"SoapClient":private]=>
  bool(false)
  ["compression":"SoapClient":private]=>
  NULL
  ["sdl":"SoapClient":private]=>
  NULL
  ["httpsocket":"SoapClient":private]=>
  NULL
  ["httpurl":"SoapClient":private]=>
  NULL
  ["_login":"SoapClient":private]=>
  NULL
  ["_password":"SoapClient":private]=>
  NULL
  ["_use_digest":"SoapClient":private]=>
  bool(false)
  ["_digest":"SoapClient":private]=>
  NULL
  ["_proxy_host":"SoapClient":private]=>
  NULL
  ["_proxy_port":"SoapClient":private]=>
  NULL
  ["_proxy_login":"SoapClient":private]=>
  NULL
  ["_proxy_password":"SoapClient":private]=>
  NULL
  ["_exceptions":"SoapClient":private]=>
  bool(true)
  ["_encoding":"SoapClient":private]=>
  NULL
  ["_classmap":"SoapClient":private]=>
  NULL
  ["_features":"SoapClient":private]=>
  NULL
  ["_connection_timeout":"SoapClient":private]=>
  int(0)
  ["_stream_context":"SoapClient":private]=>
  resource(%d) of type (stream-context)
  ["_user_agent":"SoapClient":private]=>
  NULL
  ["_keep_alive":"SoapClient":private]=>
  bool(true)
  ["_ssl_method":"SoapClient":private]=>
  NULL
  ["_soap_version":"SoapClient":private]=>
  int(1)
  ["_use_proxy":"SoapClient":private]=>
  NULL
  ["_cookies":"SoapClient":private]=>
  array(0) {
  }
  ["__default_headers":"SoapClient":private]=>
  NULL
  ["__soap_fault":"SoapClient":private]=>
  NULL
  ["__last_request":"SoapClient":private]=>
  NULL
  ["__last_response":"SoapClient":private]=>
  NULL
  ["__last_request_headers":"SoapClient":private]=>
  NULL
  ["__last_response_headers":"SoapClient":private]=>
  NULL
}

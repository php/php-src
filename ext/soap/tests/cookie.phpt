--TEST--
SOAP: SoapClient::__setCookie() and SoapClient::__getCookies()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function dump_cookies($client) {
	$c = $client->__getCookies();
	$client->test();
	if (preg_match('/^Cookie:[^\n\r]*/mi', $client->__getLastRequestHeaders(), $r)) {
		echo $r[0]."\n";
	} else {
		echo "Cookie:\n";
	}
	foreach ($c as $key => $val) {
		if (isset($val[0])) {
			echo "  ".gettype($key)."(\"".$key."\") => ".gettype($val[0])."(\"".$val[0]."\")\n";
		}
	}
}

ini_set('default_socket_timeout', 0.1);
$server = stream_socket_server('tcp://127.0.0.1:31337');
$client = new SoapClient(NULL, array("location"=>"http://127.0.0.1:31337","uri"=>"test://",
  "exceptions"=>0,"trace"=>1, "connection_timeout"=>1));
dump_cookies($client);
$client->__setCookie("var1","val1");
dump_cookies($client);
$client->__setCookie("var2","val2");
dump_cookies($client);
$client->__setCookie("var1");
$client->__setCookie("var2","val3");
dump_cookies($client);
$client->__setCookie("var2");
dump_cookies($client);
?>
--EXPECT--
Cookie:
Cookie: var1=val1;
  unicode("var1") => unicode("val1")
Cookie: var1=val1;var2=val2;
  unicode("var1") => unicode("val1")
  unicode("var2") => unicode("val2")
Cookie: var2=val3;
  unicode("var2") => unicode("val3")
Cookie:

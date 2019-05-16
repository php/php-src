--TEST--
SOAP Bug #69137 - Peer verification fails when using a proxy with SoapClient
--SKIPIF--
<?php
require_once('skipif.inc');
if (getenv("SKIP_ONLINE_TESTS")) { die("skip test requiring internet connection"); }
if (!getenv('http_proxy')) { die("skip test unless an HTTP/HTTPS proxy server is specified in http_proxy environment variable"); }
?>
--INI--
soap.wsdl_cache_enabled=1
--FILE--
<?php

class IpLookup
{
    public $licenseKey;
    public $ipAddress;
}

list ($proxyHost, $proxyPort) = explode(':', str_replace('http://', '', $_ENV['http_proxy']));

// Prime the WSDL cache because that request sets peer_name on the HTTP context
// and masks the SOAP bug.
$testServiceWsdl = 'https://ws.cdyne.com/ip2geo/ip2geo.asmx?wsdl';
$client = new SoapClient($testServiceWsdl);
unset($client);

$parameters = [
	'proxy_host' => $proxyHost,
	'proxy_port' => $proxyPort,
	'trace' => 1,
];
$client = new SoapClient($testServiceWsdl, $parameters);

$lookup = new IpLookup();
$lookup->licenseKey = 0;
$lookup->ipAddress = '72.52.91.14';

$result = $client->ResolveIP($lookup);

if ($result && is_object($result) && $result->ResolveIPResult && is_object($result->ResolveIPResult)) {
	print "successful lookup";
}
?>
--EXPECT--
successful lookup

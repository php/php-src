--TEST--
SoapServer::handle() must not free the session persisted object when a header handler returns a SoapFault
--EXTENSIONS--
soap
session
--INI--
session.save_handler=files
session.use_cookies=0
session.use_strict_mode=0
soap.wsdl_cache_enabled=0
--FILE--
<?php
class Handler
{
    public $property = 'kept';

    public function __destruct()
    {
        echo 'Handler::__destruct', PHP_EOL;
    }

    public function myHeader($x)
    {
        return new SoapFault('Server', 'header fault');
    }

    public function existing()
    {
        return 'ok';
    }
}

ini_set('session.save_path', __DIR__);
session_id('soapPersistenceHeaderFault');
session_start();

$server = new SoapServer(null, ['uri' => 'http://test-uri']);
$server->setClass('Handler');
$server->setPersistence(SOAP_PERSISTENCE_SESSION);

$request = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
  <SOAP-ENV:Header><ns1:myHeader xmlns:ns1="http://test-uri"><x>1</x></ns1:myHeader></SOAP-ENV:Header>
  <SOAP-ENV:Body><ns1:existing xmlns:ns1="http://test-uri"/></SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;

$server->handle($request);

echo 'after handle', PHP_EOL;
var_dump($_SESSION['_bogus_session_name']->property);
echo 'end', PHP_EOL;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/sess_soapPersistenceHeaderFault');
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>header fault</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
after handle
string(4) "kept"
end
Handler::__destruct

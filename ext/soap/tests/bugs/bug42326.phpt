--TEST--
Bug #42326 (SoapServer crash)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$request = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://www.example.com/"><SOAP-ENV:Body><ns1:GetProductsRequest><time></time></ns1:GetProductsRequest></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;


$soap_admin_classmap = array('productDetailsType' => 'SOAP_productDetailsType',
                             'GetProductsRequest' => 'SOAP_GetProductsRequest',
                             'GetProductsResponse' => 'SOAP_GetProductsResponse');

class SOAP_productDetailsType {
    public $id = 0;
}

class SOAP_GetProductsRequest {
    public $time = '';
}

class SOAP_GetProductsResponse {
    public $products;
    function __construct(){
        $this->products = new SOAP_productDetailsType();

    }
}

class SOAP_Admin {
    public function GetProducts($time){
        return new SOAP_GetProductsResponse();
    }
}

$soap = new SoapServer(__DIR__.'/bug42326.wsdl', array('classmap' => $soap_admin_classmap));
$soap->setClass('SOAP_Admin');
ob_start();
$soap->handle($request);
ob_end_clean();
echo "ok\n";
?>
--EXPECT--
ok

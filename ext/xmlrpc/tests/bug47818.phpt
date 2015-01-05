--TEST--
Bug #47818 (Segfault due to bound callback param)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

class MyXmlRpc {
    private $s;
    private $method;

    function impl($method_name, $params, $user_data){
        $this->method = $method_name;
        print "Inside impl(): {$this->method}\n";
        return array_sum($params);
    }

    function __construct() {
        $this->s = xmlrpc_server_create();
        xmlrpc_server_register_method($this->s, 'add', array($this, 'impl'));
    }

    function call($req) {
        return xmlrpc_server_call_method($this->s, $req, null);
    }

    function getMethod() {return $this->method;}

}

$x = new MyXmlRpc;
$resp = $x->call(xmlrpc_encode_request('add', array(1, 2, 3)));

$method = $x->getMethod();

print "Global scope: $method\n";

?>
--EXPECTF--
Inside impl(): add
Global scope: add

--TEST--
Bug #42736 (xmlrpc_server_call_method() crashes)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

class SOAP_Array {
	public function get($id){
		return $this->add($id);
	}
}

$xml = xmlrpc_server_create();

$Myrequest = '<?xml version="1.0" encoding="UTF-8"?><methodCall><methodName>GetProducts</methodName><params><param><value><dateTime.iso8601>20060922T14:26:19</dateTime.iso8601></value></param></params></methodCall>';

class MyClass {
	function GetProducts($dummy, $time){
		return array('faultString' => $time);
	}
}
$myclass =  new MyClass();
xmlrpc_server_register_method($xml, 'GetProducts', array($myclass, 'GetProducts'));
$response = xmlrpc_server_call_method($xml, $Myrequest, null);

var_dump($response);

echo "Done\n";
?>
--EXPECTF--	
string(402) "<?xml version="1.0" encoding="iso-8859-1"?>
<methodResponse>
<params>
 <param>
  <value>
   <struct>
    <member>
     <name>faultString</name>
     <value>
      <array>
       <data>
        <value>
         <dateTime.iso8601>20060922T14:26:19</dateTime.iso8601>
        </value>
       </data>
      </array>
     </value>
    </member>
   </struct>
  </value>
 </param>
</params>
</methodResponse>
"
Done

--TEST--
Bug #50761 (system.multiCall crashes)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php
$req = '<?xml version="1.0"?>
<methodCall>
<methodName>system.multiCall</methodName>
<params><param><value><array><data>
<value><struct>
<member><name>methodName</name><value><string>testMethodA</string></value></member>
<member><name>params</name><value><array><data><value><string>A</string>
</value></data></array></value></member>
</struct></value>
<value><struct>
<member><name>methodName</name><value><string>testMethodB</string></value></member>
<member><name>params</name><value><array><data><value><string>B</string>
</value></data></array></value></member>
</struct></value>
</data></array></value></param></params>
</methodCall>';

function testA($methodName, $params, $var){ return "C"; }
function testB($methodName, $params, $var){ return "D"; }

$server = xmlrpc_server_create();
xmlrpc_server_register_method($server, 'testMethodA', 'testA');
xmlrpc_server_register_method($server, 'testMethodB', 'testB');
$res = xmlrpc_server_call_method($server, $req, null);
echo $res;
?>
--EXPECT--
<?xml version="1.0" encoding="iso-8859-1"?>
<methodResponse>
<params>
 <param>
  <value>
   <array>
    <data>
     <value>
      <array>
       <data>
        <value>
         <string>C</string>
        </value>
       </data>
      </array>
     </value>
     <value>
      <array>
       <data>
        <value>
         <string>D</string>
        </value>
       </data>
      </array>
     </value>
    </data>
   </array>
  </value>
 </param>
</params>
</methodResponse>

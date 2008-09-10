--TEST--
Bug #45226 (xmlrpc_set_type() segfaults with valid ISO8601 date string)
--INI--
date.timezone="America/Sao_Paulo"
--FILE--
<?php

$d = date(DATE_ISO8601);
xmlrpc_set_type($d, 'datetime');
echo xmlrpc_encode_request('method.call', array('date' => $d));

$d = '2008-01-01 20:00:00';
xmlrpc_set_type($d, 'datetime');
echo xmlrpc_encode_request('method.call', array('date' => $d));

?>
--EXPECTF--
<?xml version="1.0" encoding="iso-8859-1"?>
<methodCall>
<methodName>method.call</methodName>
<params>
 <param>
  <value>
   <struct>
    <member>
     <name>date</name>
     <value>
      <dateTime.iso8601>%d-%d-%dT%d:%d:%d%s%d</dateTime.iso8601>
     </value>
    </member>
   </struct>
  </value>
 </param>
</params>
</methodCall>
<?xml version="1.0" encoding="iso-8859-1"?>
<methodCall>
<methodName>method.call</methodName>
<params>
 <param>
  <value>
   <struct>
    <member>
     <name>date</name>
     <value>
      <dateTime.iso8601>%d-%d-%d %d:%d:%d</dateTime.iso8601>
     </value>
    </member>
   </struct>
  </value>
 </param>
</params>
</methodCall>

--TEST--
Bug #37057 (xmlrpc_decode() may produce arrays with numeric string keys which are unaccessible)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php
$response='<?xml version="1.0"?>
<methodResponse>
  <params>
    <param>
      <value>
        <struct>
          <member>
            <name>50</name>
            <value><string>0.29</string></value>
          </member>
        </struct>
      </value>
    </param>
  </params>
</methodResponse>';

$retval=xmlrpc_decode($response);
var_dump($retval);
var_dump($retval["50"]);
var_dump($retval[50]);

$response='<?xml version="1.0"?>
<methodResponse>
  <params>
    <param>
      <value>
        <struct>
          <member>
            <name>0</name>
            <value><string>0.29</string></value>
          </member>
        </struct>
      </value>
    </param>
  </params>
</methodResponse>';

$retval=xmlrpc_decode($response);
var_dump($retval);
var_dump($retval["0"]);
var_dump($retval[0]);

echo "Done\n";
?>
--EXPECT--	
array(1) {
  [50]=>
  string(4) "0.29"
}
string(4) "0.29"
string(4) "0.29"
array(1) {
  [0]=>
  string(4) "0.29"
}
string(4) "0.29"
string(4) "0.29"
Done

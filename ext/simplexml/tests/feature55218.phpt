--TEST--
Bug #55218 getDocNamespaces from current element and not root
--SKIPIF--
<?php
if (!extension_loaded("simplexml")) print "skip SimpleXML not present";
if (!extension_loaded("libxml")) print "skip LibXML not present";
?>
--FILE--
<?php 

$x = new SimpleXMLElement(
'<?xml version="1.0" standalone="yes"?>
<people xmlns:p="http://example.org/p" >
    <person id="1" xmlns:t="http://example.org/t" >
                <t:name>John Doe</t:name>
        </person>
    <person id="2">Susie Q. Public</person>
    <o>
                <p:div>jdslkfjsldk jskdfjsmlkjfkldjkjflskj kljfslkjf sldk</p:div>
        </o>
</people>');

echo "getDocNamespaces\n";
echo "\nBackwards Compatibility:\n";
echo "recursion:\n";

var_dump ( $x->getDocNamespaces(true) ) ;
var_dump( $x->person[0]->getDocNamespaces(true) );
var_dump( $x->person[1]->getDocNamespaces(true) );

echo "\nnon recursive:\n";

var_dump( $x->getDocNamespaces(false) );
var_dump( $x->person[0]->getDocNamespaces(false) );
var_dump( $x->person[1]->getDocNamespaces(false) );

echo "\n\nUsing new 'from_root' bool set to false:\n";
echo "recursion:\n";

var_dump ( $x->getDocNamespaces(true, false) ) ;
var_dump( $x->person[0]->getDocNamespaces(true, false) );
var_dump( $x->person[1]->getDocNamespaces(true, false) );

echo "\nnon recursive:\n";

var_dump( $x->getDocNamespaces(false, false) );
var_dump( $x->person[0]->getDocNamespaces(false, false) );
var_dump( $x->person[1]->getDocNamespaces(false, false) );

?>
===DONE===
--EXPECTF--
getDocNamespaces

Backwards Compatibility:
recursion:
array(2) {
  ["p"]=>
  string(20) "http://example.org/p"
  ["t"]=>
  string(20) "http://example.org/t"
}
array(2) {
  ["p"]=>
  string(20) "http://example.org/p"
  ["t"]=>
  string(20) "http://example.org/t"
}
array(2) {
  ["p"]=>
  string(20) "http://example.org/p"
  ["t"]=>
  string(20) "http://example.org/t"
}

non recursive:
array(1) {
  ["p"]=>
  string(20) "http://example.org/p"
}
array(1) {
  ["p"]=>
  string(20) "http://example.org/p"
}
array(1) {
  ["p"]=>
  string(20) "http://example.org/p"
}


Using new 'from_root' bool set to false:
recursion:
array(2) {
  ["p"]=>
  string(20) "http://example.org/p"
  ["t"]=>
  string(20) "http://example.org/t"
}
array(1) {
  ["t"]=>
  string(20) "http://example.org/t"
}
array(0) {
}

non recursive:
array(1) {
  ["p"]=>
  string(20) "http://example.org/p"
}
array(1) {
  ["t"]=>
  string(20) "http://example.org/t"
}
array(0) {
}
===DONE===
--TEST--
Test Uri\QueryParams::append() - success - LF, CR, and CRLF in names and values
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("a\nb\rc\r\nd", "e\nf\rg\r\nh");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECTF--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(8) "a%r\n%rb%r\r%rc%r\r?\n%rd"
    [1]=>
    string(8) "e%r\n%rf%r\r%rg%r\r?\n%rh"
  }
}
string(45) "a%r\x25%r0D%r\x25%r0Ab%r\x25%r0D%r\x25%r0Ac%r\x25%r0D%r\x25%r0Ad=e%r\x25%r0D%r\x25%r0Af%r\x25%r0D%r\x25%r0Ag%r\x25%r0D%r\x25%r0Ah"
string(33) "a%r\x25%r0Ab%r\x25%r0Dc%r\x25%r0D%r\x25%r0Ad=e%r\x25%r0Af%r\x25%r0Dg%r\x25%r0D%r\x25%r0Ah"
string(33) "a%r\x25%r0Ab%r\x25%r0Dc%r\x25%r0D%r\x25%r0Ad=e%r\x25%r0Af%r\x25%r0Dg%r\x25%r0D%r\x25%r0Ah"

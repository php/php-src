--TEST--
Test imap_bodystruct() function : basic functionality 
--SKIPIF--
<?php 
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : object imap_bodystruct  ( resource $imap_stream  , int $msg_number  , string $section  )
 * Description: Read the structure of a specified body section of a specific message.
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing string imap_bodystruct : basic functionality ***\n";
require_once(dirname(__FILE__).'/imap_include.inc');

echo "Create a new mailbox for test and add a multipart msgs\n";
$imap_stream = setup_test_mailbox("", 1, $mailbox, "multipart");
if (!is_resource($imap_stream)) {
	exit("TEST FAILED: Unable to create test mailbox\n");
}

echo "\nGet and validate structure of body part 1\n"; 

$m = imap_bodystruct($imap_stream, 1, "1"); 
 
$mandatoryFields = array(
                    'ifsubtype',
                    'ifdescription',
                    'ifid',
                    'ifdisposition',
                    'ifdparameters',
                    'ifparameters',
                    );

foreach($mandatoryFields as $mf) 
{
  if(isValid($m->$mf)) 
  {
    echo "$mf is 0 or 1\n";
  }
  else
  {
    echo "$mf FAIL\n";
  }
}

if(is_array($m->parameters)) 
{
  echo "parameters is an array\n";
}

echo "\nTry to get part 4!\n";
var_dump(imap_bodystruct($imap_stream, 1, "4")); 

imap_close($imap_stream);

function isValid($param) 
{
 if(($param == 0) || ($param == 1)) 
 {
   $result=true;
 }
 else
 {
   $result=false;
 } 
return $result;
}
?>
===Done===
--CLEAN--
<?php 
require_once('clean.inc');
?>
--EXPECTF--
*** Testing string imap_bodystruct : basic functionality ***
Create a new mailbox for test and add a multipart msgs
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created

Get and validate structure of body part 1
ifsubtype is 0 or 1
ifdescription is 0 or 1
ifid is 0 or 1
ifdisposition is 0 or 1
ifdparameters is 0 or 1
ifparameters is 0 or 1
parameters is an array

Try to get part 4!
bool(false)
===Done===

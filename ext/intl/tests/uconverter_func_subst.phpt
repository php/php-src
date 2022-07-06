--TEST--
Basic UConverter::convert() w/ Subsitution
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--INI--
intl.use_exceptions=false
--FILE--
<?php
foreach(array('?','','??') as $subst) {
  $opts = array('to_subst' => $subst);
  $ret = UConverter::transcode("This is an ascii string", 'ascii', 'utf-8', $opts);
  if ($ret === FALSE) {
    echo "Error: ", intl_get_error_message(), "\n";
  } else {
    var_dump($ret);
  }
  $ret = UConverter::transcode("Snowman: (\xE2\x98\x83)", 'ascii', 'utf-8', $opts);
  if ($ret === FALSE) {
    echo "Error: ", intl_get_error_message(), "\n";
  } else {
    var_dump($ret);
  }
}
?>
--EXPECT--
string(23) "This is an ascii string"
string(12) "Snowman: (?)"
Error: transcode() returned error 1: U_ILLEGAL_ARGUMENT_ERROR: U_ILLEGAL_ARGUMENT_ERROR
Error: transcode() returned error 1: U_ILLEGAL_ARGUMENT_ERROR: U_ILLEGAL_ARGUMENT_ERROR
Error: transcode() returned error 1: U_ILLEGAL_ARGUMENT_ERROR: U_ILLEGAL_ARGUMENT_ERROR
Error: transcode() returned error 1: U_ILLEGAL_ARGUMENT_ERROR: U_ILLEGAL_ARGUMENT_ERROR

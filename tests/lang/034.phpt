--TEST--
Locale settings affecting float parsing
--SKIPIF--
<?php  # try to activate a german locale
$status = false;
foreach(array("de_DE", "de", "german", "ge") as $lang) {
  if($lang == setlocale(LC_NUMERIC, $lang) {
	  $status = true; 
	continue;
  }
}
if(!$status) print "skip";
?>
--POST--
--GET--
--FILE--
<?php 
# try to activate a german locale
foreach(array("de_DE", "de", "german", "ge") as $lang) {
  if($lang == setlocale(LC_NUMERIC, $lang)) {
	  continue;
  }
}

echo (float)"3.14", "\n";

?>
--EXPECT--
3.14

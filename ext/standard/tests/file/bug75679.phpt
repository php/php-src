--TEST--
Bug #75679 Path 260 character problem
--FILE--
<?php
$d = __DIR__;
$Files2Report =
 [
  str_pad ($d . '/' . str_repeat (str_pad ('bug75679_path_259_characters_long_', 100, '_') . '/', 1), 259, '_') => [],
  str_pad ($d . '/' . str_repeat (str_pad ('bug75679_path_260_characters_long_', 100, '_') . '/', 1), 260, '_') => [],
  str_pad ($d . '/' . str_repeat (str_pad ('bug75679_path_261_characters_long_', 100, '_') . '/', 1), 261, '_') => [],
 ];
foreach ($Files2Report as $file => &$Report)
 {
  $Report = ['strlen' => strlen ($file), 'result' => 'nok'];

  if (! is_dir (dirname ($file))) mkdir (dirname ($file), 0777, true);
  if (copy (__FILE__, $file) && is_file ($file))
   {
    $Report['result'] = 'ok';
   }

  print_r ($Report);
 }


?>
==DONE==
--CLEAN--
<?php
$d = __DIR__;
$Files2Report =
 [
  str_pad ($d . '/' . str_repeat (str_pad ('bug75679_path_259_characters_long_', 100, '_') . '/', 1), 259, '_') => [],
  str_pad ($d . '/' . str_repeat (str_pad ('bug75679_path_260_characters_long_', 100, '_') . '/', 1), 260, '_') => [],
  str_pad ($d . '/' . str_repeat (str_pad ('bug75679_path_261_characters_long_', 100, '_') . '/', 1), 261, '_') => [],
 ];
foreach ($Files2Report as $file => &$Report)
{
	if (is_file($file)) {
		unlink($file);
	}

	$d1 = dirname($file);
	while ($d1 != $d) {
		rmdir($d1);
		$d1 = dirname($d1);
	}
}
?>
--EXPECT--
Array
(
    [strlen] => 259
    [result] => ok
)
Array
(
    [strlen] => 260
    [result] => ok
)
Array
(
    [strlen] => 261
    [result] => ok
)
==DONE==

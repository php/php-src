<?
$module = 'sockets';
$function = 'confirm_' . $module . '_compiled';
if (extension_loaded($module)) {
	$str = $function($module);
} else {
	$str = "Module $module is not compiled in PHP";
}
echo "$str\n";
?>

<?
$module = '_extname_';
$function = $module . '_test';
if (extension_loaded($module)) {
	$str = $function($module);
} else {
	$str = "Module $module is not compiled in PHP";
}
echo "$str\n";
?>

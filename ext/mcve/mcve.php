<?
$module = 'MCVE';
if(!extension_loaded($module)) {
	dl('mcve.so');
}
$functions = get_extension_funcs($module);
echo "Functions available in the $module extension:<br>\n";
foreach($functions as $func) {
    echo $func."<br>\n";
}
echo "<br>\n";
?>

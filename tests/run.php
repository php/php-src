<?
if (!isset($code)) {
	exit("No code submitted.");
}
?>
<html>
<body>
Executing:<br>
<?
highlight_string("<?php \n$code\n?>");
?>
<hr width="40%">
<?
eval($code);
?>
</body>
</html>
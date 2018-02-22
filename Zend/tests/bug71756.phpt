--TEST--
Bug #71756 (Call-by-reference widens scope to uninvolved functions when used in switch)
--FILE--
<?php
function a ($option) {
	b($option['bla']);
	c($option);
	var_dump($option);
}
function b (&$string) {
	$string = 'changed';
}
function c ($option) {
	switch ($option['bla']) {
	case 'changed':
		$copy = $option;
		$copy['bla'] = 'copy';
		break;
	}
}
a(array('bla' => 'false'));
?>
--EXPECT--
array(1) {
  ["bla"]=>
  string(7) "changed"
}

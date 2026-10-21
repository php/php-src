--TEST--
CVE-2026-109618 singleton input arrays grow, convert, copy and sort correctly
--FILE--
<?php
function check($condition) {
	if (!$condition) {
		throw new Exception("failed");
	}
}
foreach (array('a', '0', '7', '999') as $first) {
	$fields = array();
	$expected = array();
	$fields[] = 'root[' . $first . ']=first';
	$expected[$first] = 'first';
	for ($i = 0; $i < 150; $i++) {
		$key = $i % 2 ? 'key' . $i : $i;
		$fields[] = 'root[' . $key . ']=' . $i;
		$expected[$key] = (string) $i;
	}
	parse_str(implode('&', $fields), $out);
	check($out['root'] === $expected);
	$copy = $out;
	$out['root']['extra'] = 'new';
	check(!isset($copy['root']['extra']));
	check(unserialize(serialize($copy)) === $copy);
	ksort($copy['root']);
	ksort($expected);
	check($copy['root'] === $expected);
	foreach (array_keys($copy['root']) as $key) {
		unset($copy['root'][$key]);
	}
	$copy['root'][] = 'after-empty';
	check(count($copy['root']) === 1);
}
parse_str('x=old&x[a][b][c]=new&list[][]=one&list[][]=two', $out);
check($out === array('x' => array('a' => array('b' => array('c' => 'new'))),
	'list' => array(array('one'), array('two'))));
echo "OK\n";
?>
--EXPECT--
OK

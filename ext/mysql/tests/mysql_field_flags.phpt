--TEST--
mysql_field_flags()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$tmp    = NULL;
$link   = NULL;

if (!is_null($tmp = @mysql_field_flags()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (null !== ($tmp = @mysql_field_flags($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');
if (!$res = mysql_query("SELECT id, label FROM test ORDER BY id LIMIT 2", $link)) {
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

if (NULL !== ($tmp = mysql_field_flags($res)))
	printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (false !== ($tmp = mysql_field_flags($res, -1)))
	printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if (!is_string($tmp = mysql_field_flags($res, 0)) || empty($tmp))
	printf("[006] Expecting non empty string, got %s/%s\n", gettype($tmp), $tmp);

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && !is_unicode($tmp)) {
	printf("[007] Check the unicode support!\n");
	var_inspect($tmp);
}

if (false !== ($tmp = mysql_field_flags($res, 2)))
	printf("[008] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

mysql_free_result($res);

$version = mysql_get_server_info($link);
if (!preg_match('@(\d+)\.(\d+)\.(\d+)@ism', $version, $matches))
	printf("[009] Cannot get server version\n");
$version = ($matches[1] * 100) + ($matches[2] * 10) + $matches[3];

$tables = array(
	'label INT, UNIQUE KEY (label)'                         =>  array(
								array('label', '1'),
								'label' => array(($version < 500) ? 'multiple_key' : 'unique_key')
								),
	'labela INT, label2 CHAR(1), KEY keyname (labela, label2)'      =>  array(
								array('labela, label2', "1, 'a'"),
								'labela' => array('multiple_key'),
								),
	'label1 BLOB'                                           =>  array(
								array('label1', "'blob'"),
								'label1' => array('blob', 'binary'),
								),
	'label1 INT UNSIGNED'                                   =>  array(
								array('label1', '100'),
								'label1' => array('unsigned'),
								),
	'label1 INT UNSIGNED NOT NULL AUTO INCREMENT'           =>  array(
								array('label1', '100'),
								'label1' => array('auto_increment',
										'unsigned'),
								),
	'label1 ENUM("a", "b")'                                 =>  array(
								array('label1', "'a'"),
								'label1' => array('enum'),
								),
	'label1 SET("a", "b")'                                  =>  array(
								array('label1', "'a'"),
								'label1' => array('set'),
								),
	'label1 TIMESTAMP'                                      =>  array(
								array('label1', sprintf("'%s'", @date("Y-m-d H:i:s"))),
								'label1' => array(
										'timestamp',
										'unsigned',
										'zerofill',
										'binary',
										'not_null'),
								),
);

foreach ($tables as $columns => $expected) {
	if (!mysql_query("DROP TABLE IF EXISTS test", $link)) {
		printf("[010/%s] [%d] %s\n", $columns, mysql_errno($link), mysql_error($link));
		continue;
	}
	$sql = sprintf("CREATE TABLE test(id INT, %s) ENGINE = %s", $columns, $engine);
		if (!@mysql_query($sql, $link)) {
		// server or engine might not support this
		continue;
	}

	reset($expected);
	list($k, $values) = each($expected);
	$sql = sprintf("INSERT INTO test(id, %s) VALUES (1, %s)", $values[0], $values[1]);
	if (!mysql_query($sql, $link)) {
		printf("[011/%s] '%s', [%d] %s\n", $columns, $sql, mysql_errno($link), mysql_error($link));
		continue;
	}

	if (!$res = mysql_query(sprintf("SELECT id, %s FROM test", $values[0]), $link)) {
		printf("[012/%s] [%d] %s\n", $columns, mysql_errno($link), mysql_error($link));
		continue;
	}

	$i = 1;
	while (list($field, $flags) = each($expected)) {
		$tmp = mysql_field_flags($res, $i++);

		foreach ($flags as $k => $flag) {
		if (!preg_match(sprintf('@\s*%s\s*@ismU', $flag), $tmp)) {
			printf("[013/%s] Field '%s', flag '%s' not found, [%d] %s\n", $columns, $field, $flag, mysql_errno($link), mysql_error($link));
		}
	}
		foreach ($flags as $k => $flag) {
			$tmp = preg_replace(sprintf('@\s*%s\s*@ismU', $flag), '', $tmp);
		}
		if ('' != $tmp)
			printf("[014/%s] Field '%s', unexpected flags '%s' found, [%d] %s\n", $columns, $field, $tmp, mysql_errno($link), mysql_error($link));
	}
	mysql_free_result($res);
}

var_dump(mysql_field_flags($res, 0));

mysql_close($link);
print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysql_field_flags() expects exactly 2 parameters, 1 given in %s on line %d

Warning: mysql_field_flags(): Field -1 is invalid for MySQL result index %d in %s on line %d

Warning: mysql_field_flags(): Field 2 is invalid for MySQL result index %d in %s on line %d

Warning: mysql_field_flags(): %d is not a valid MySQL result resource in %s on line %d
bool(false)
done!

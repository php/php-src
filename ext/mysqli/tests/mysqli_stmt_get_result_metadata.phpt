--TEST--
mysqli_stmt_get_result() - meta data
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_stmt_get_result'))
	die('skip mysqli_stmt_get_result not available');
?>
--FILE--
<?php
	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test ORDER BY id ASC LIMIT 3"))
		printf("[002] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[003] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
		printf("[004] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
			gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	if (!is_object($res_meta = mysqli_stmt_result_metadata($stmt)) ||
		'mysqli_result' != get_class($res_meta)) {
		printf("[005] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
			gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	var_dump(mysqli_fetch_assoc($res));
	var_dump(mysqli_fetch_assoc($res_meta));

	mysqli_free_result($res);
	mysqli_free_result($res_meta);
	mysqli_stmt_close($stmt);

	// !mysqli_stmt_prepare($stmt, "SELECT id, label, id + 1 as _id,  concat(label, '_') _label FROM test as _test ORDER BY id ASC LIMIT 3") ||
	if (!($stmt = mysqli_stmt_init($link)) ||
		!mysqli_stmt_prepare($stmt, "SELECT id , label, id + 1 AS _id, label AS _label, null AS _null, CONCAT(label, '_') _label_concat  FROM test _test ORDER BY id ASC LIMIT 3") ||
		!mysqli_stmt_execute($stmt))
		printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!is_object($res = mysqli_stmt_get_result($stmt)) || 'mysqli_result' != get_class($res)) {
		printf("[007] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
			gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	if (!is_object($res_meta = mysqli_stmt_result_metadata($stmt)) ||
		'mysqli_result' != get_class($res_meta)) {
		printf("[008] Expecting object/mysqli_result got %s/%s, [%d] %s\n",
			gettype($res), $res, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	if (($tmp1 = mysqli_num_fields($res)) !== ($tmp2 = mysqli_num_fields($res_meta))) {
		printf("[009] %s/%s !== %s/%s\n", gettype($tmp1), $tmp1, gettype($tmp2), $tmp2);
	}

	/*
	if (($tmp1 = mysqli_field_count($link)) !== ($tmp2 = $res->field_count()))
		printf("[010] %s/%s !== %s/%s\n", gettype($tmp1), $tmp1, gettype($tmp2), $tmp2);

	if (($tmp1 = $res_meta->field_count()) !== $tmp2)
		printf("[011] %s/%s !== %s/%s\n", gettype($tmp1), $tmp1, gettype($tmp2), $tmp2);
	*/

	if (($tmp1 = mysqli_field_tell($res)) !== ($tmp2 = $res_meta->current_field))
		printf("[012] %s/%s !== %s/%s\n", gettype($tmp1), $tmp1, gettype($tmp2), $tmp2);

	if (0 !== $tmp1)
		printf("[013] Expecting int/0 got %s/%s\n", gettype($tmp1), $tmp1);

	$fields = array();
	while ($info = $res->fetch_field())
		$fields['res'][] = $info;
	var_dump($info);
	while ($info = $res_meta->fetch_field())
		$fields['meta'][] = $info;
	var_dump($info);
	$fields['all_res'] = $res->fetch_fields();
	$fields['all_meta'] = $res_meta->fetch_fields();

	if (count($fields['res']) != count($fields['meta'])) {
		printf("[014] stmt_get_result indicates %d fields, stmt_result_metadata indicates %d fields\n",
			count($fields['res']),
			count($fields['meta']));
	}

	foreach ($fields['res'] as $k => $info) {
		printf("%s\n", $info->name);
		if ($info->name !== $fields['meta'][$k]->name)
			printf("[015 - %d] Expecting name %s/%s got %s/%s\n",
				$k, gettype($info->name), $info->name, gettype($fields['meta'][$k]->name), $fields['meta'][$k]->name);

		if ($info->orgname !== $fields['meta'][$k]->orgname)
			printf("[016 - %d] Expecting orgname %s/%s got %s/%s\n",
				$k, gettype($info->orgname), $info->orgname, gettype($fields['meta'][$k]->orgname), $fields['meta'][$k]->orgname);

		if ($info->table !== $fields['meta'][$k]->table)
			printf("[017 - %d] Expecting table %s/%s got %s/%s\n",
				$k, gettype($info->table), $info->table, gettype($fields['meta'][$k]->table), $fields['meta'][$k]->table);

		if ($info->orgtable !== $fields['meta'][$k]->orgtable)
			printf("[018 - %d] Expecting orgtable %s/%s got %s/%s\n",
				$k, gettype($info->orgtable), $info->orgtable, gettype($fields['meta'][$k]->orgtable), $fields['meta'][$k]->orgtable);

		if ($info->def !== $fields['meta'][$k]->def)
			printf("[019 - %d] Expecting def %s/%s got %s/%s\n",
				$k, gettype($info->def), $info->def, gettype($fields['meta'][$k]->def), $fields['meta'][$k]->def);
/*
		if ($info->max_length !== $fields['meta'][$k]->max_length)
			printf("[020 - %d] Expecting max_length %s/%s got %s/%s\n",
				$k, gettype($info->max_length), $info->max_length, gettype($fields['meta'][$k]->max_length), $fields['meta'][$k]->max_length);
*/
		if ($info->length !== $fields['meta'][$k]->length)
			printf("[021 - %d] Expecting length %s/%s got %s/%s\n",
				$k, gettype($info->length), $info->length, gettype($fields['meta'][$k]->length), $fields['meta'][$k]->length);

		if ($info->charsetnr !== $fields['meta'][$k]->charsetnr)
			printf("[022 - %d] Expecting charsetnr %s/%s got %s/%s\n",
				$k, gettype($info->charsetnr), $info->charsetnr, gettype($fields['meta'][$k]->charsetnr), $fields['meta'][$k]->charsetnr);

		if ($info->flags !== $fields['meta'][$k]->flags)
			printf("[023 - %d] Expecting flags %s/%s got %s/%s\n",
				$k, gettype($info->flags), $info->flags, gettype($fields['meta'][$k]->flags), $fields['meta'][$k]->flags);

		if ($info->type !== $fields['meta'][$k]->type)
			printf("[024 - %d] Expecting type %s/%s got %s/%s\n",
				$k, gettype($info->type), $info->type, gettype($fields['meta'][$k]->type), $fields['meta'][$k]->type);

		if ($info->decimals !== $fields['meta'][$k]->decimals)
			printf("[025 - %d] Expecting decimals %s/%s got %s/%s\n",
				$k, getdecimals($info->decimals), $info->decimals, getdecimals($fields['meta'][$k]->decimals), $fields['meta'][$k]->decimals);

		/* Make them equal for the check */
		$tmp = $fields['all_res'][$k]->max_length;
		$fields['all_res'][$k]->max_length = $fields['all_meta'][$k]->max_length;

		if ($fields['all_res'][$k] != $fields['all_meta'][$k]) {
			printf("[026 - %d] fetch_fields() seems to have returned different data, dumping\n", $k);
			var_dump($fields['all_res'][$k]);
			var_dump($fields['all_meta'][$k]);
		}
		$fields['all_res'][$k]->max_length = $tmp;
	}

	$num = count($fields['res']);
	for ($i = 0; $i < 100; $i++) {
		$pos = mt_rand(-1, $num + 1);
		if ($pos >= 0 && $pos < $num) {
			if (true !== mysqli_field_seek($res, $pos))
				printf("[027] field_seek(res) failed\n");
			if (true !== $res_meta->field_seek($pos))
				printf("[028] field_seek(res__meta) failed\n");

			$tmp1 = $res->fetch_field();
			$tmp2 = mysqli_fetch_field($res_meta);
			$tmp2->max_length = $tmp1->max_length;
			if ($tmp1 != $tmp2) {
				printf("[029] Field info differs, dumping data\n");
				var_dump($tmp1);
				var_dump($tmp2);
			}

			if ($tmp1 != $fields['res'][$pos]) {
				printf("[030] Field info differs, dumping data\n");
				var_dump($tmp1);
				var_dump($fields['res'][$pos]);
			}

			$pos++;
			if ($pos !== ($tmp = mysqli_field_tell($res)))
				printf("[031] Expecting %s/%s got %s/%s\n",
					gettype($pos), $pos, gettype($tmp), $tmp);

			if ($pos !== ($tmp = mysqli_field_tell($res_meta)))
				printf("[032] Expecting %s/%s got %s/%s\n",
					gettype($pos), $pos, gettype($tmp), $tmp);
		} else {

			if (false !== @mysqli_field_seek($res, $pos))
				printf("[033] field_seek(%d) did not fail\n", $pos);
			if (false !== @mysqli_field_seek($res_meta, $pos))
				printf("[034] field_seek(%d) did not fail\n", $pos);
		}
	}

	$res->free_result();
	mysqli_free_result($res_meta);

	var_dump(mysqli_fetch_field($res));

	mysqli_stmt_close($stmt);

	var_dump(mysqli_fetch_field($res));

	mysqli_close($link);

	var_dump(mysqli_fetch_field($res));

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
array(2) {
  [%u|b%"id"]=>
  int(1)
  [%u|b%"label"]=>
  %unicode|string%(1) "a"
}
NULL
bool(false)
bool(false)
id
label
_id
_label
_null
_label_concat

Warning: mysqli_fetch_field(): Couldn't fetch mysqli_result in %s on line %d
NULL

Warning: mysqli_fetch_field(): Couldn't fetch mysqli_result in %s on line %d
NULL

Warning: mysqli_fetch_field(): Couldn't fetch mysqli_result in %s on line %d
NULL
done!
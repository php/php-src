--TEST--
mysqli_field_seek()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    function mysqli_field_seek_flags($flags) {

        $ret = '';

        if ($flags & MYSQLI_NOT_NULL_FLAG)
            $ret .= 'MYSQLI_NOT_NULL_FLAG ';

        if ($flags & MYSQLI_PRI_KEY_FLAG)
            $ret .= 'MYSQLI_PRI_KEY_FLAG ';

        if ($flags & MYSQLI_UNIQUE_KEY_FLAG)
            $ret .= 'MYSQLI_UNIQUE_KEY_FLAG ';

        if ($flags & MYSQLI_MULTIPLE_KEY_FLAG)
            $ret .= 'MYSQLI_MULTIPLE_KEY_FLAG ';

        if ($flags & MYSQLI_BLOB_FLAG)
            $ret .= 'MYSQLI_BLOB_FLAG ';

        if ($flags & MYSQLI_UNSIGNED_FLAG)
            $ret .= 'MYSQLI_UNSIGNED_FLAG ';

        if ($flags & MYSQLI_ZEROFILL_FLAG)
            $ret .= 'MYSQLI_ZEROFILL_FLAG ';

        if ($flags & MYSQLI_AUTO_INCREMENT_FLAG)
            $ret .= 'MYSQLI_AUTO_INCREMENT_FLAG ';

        if ($flags & MYSQLI_TIMESTAMP_FLAG)
            $ret .= 'MYSQLI_TIMESTAMP_FLAG ';

        if ($flags & MYSQLI_SET_FLAG)
            $ret .= 'MYSQLI_SET_FLAG ';

        if ($flags & MYSQLI_NUM_FLAG)
            $ret .= 'MYSQLI_NUM_FLAG ';

        if ($flags & MYSQLI_PART_KEY_FLAG)
            $ret .= 'MYSQLI_PART_KEY_FLAG ';

        if ($flags & MYSQLI_GROUP_FLAG)
            $ret .= 'MYSQLI_GROUP_FLAG ';

        return $ret;
    }

    require_once("connect.inc");

    require('table.inc');

    // Make sure that client, connection and result charsets are all the
    // same. Not sure whether this is strictly necessary.
    if (!mysqli_set_charset($link, 'utf8'))
        printf("[%d] %s\n", mysqli_errno($link), mysqli_errno($link));

    $charsetInfo = mysqli_get_charset($link);

    if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1", MYSQLI_USE_RESULT)) {
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    try {
        var_dump(mysqli_field_seek($res, -1));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump(mysqli_fetch_field($res));
    var_dump(mysqli_field_seek($res, 0));
    var_dump(mysqli_fetch_field($res));
    var_dump(mysqli_field_seek($res, 1));

    $field = mysqli_fetch_field($res);
    var_dump($field);
    /* label column, result set charset */
    if ($field->charsetnr != $charsetInfo->number) {
        printf("[004] Expecting charset %s/%d got %d\n",
            $charsetInfo->charset, $charsetInfo->number, $field->charsetnr);
    }
    if ($field->length != $charsetInfo->max_length) {
        printf("[005] Expecting length %d got %d\n",
            $charsetInfo->max_length, $field->max_length);
    }

    var_dump(mysqli_field_tell($res));
    try {
        var_dump(mysqli_field_seek($res, 2));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump(mysqli_fetch_field($res));

    mysqli_free_result($res);

    if (!$res = mysqli_query($link, "SELECT NULL as _null", MYSQLI_STORE_RESULT)) {
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
    var_dump(mysqli_field_seek($res, 0));
    var_dump(mysqli_fetch_field($res));

    mysqli_free_result($res);

    try {
        mysqli_field_seek($res, 0);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECTF--
mysqli_field_seek(): Argument #2 ($index) must be greater than or equal to 0
object(stdClass)#%d (13) {
  ["name"]=>
  string(2) "id"
  ["orgname"]=>
  string(2) "id"
  ["table"]=>
  string(4) "test"
  ["orgtable"]=>
  string(4) "test"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(0)
  ["length"]=>
  int(11)
  ["charsetnr"]=>
  int(63)
  ["flags"]=>
  int(49155)
  ["type"]=>
  int(3)
  ["decimals"]=>
  int(0)
}
bool(true)
object(stdClass)#%d (13) {
  ["name"]=>
  string(2) "id"
  ["orgname"]=>
  string(2) "id"
  ["table"]=>
  string(4) "test"
  ["orgtable"]=>
  string(4) "test"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(0)
  ["length"]=>
  int(11)
  ["charsetnr"]=>
  int(63)
  ["flags"]=>
  int(49155)
  ["type"]=>
  int(3)
  ["decimals"]=>
  int(0)
}
bool(true)
object(stdClass)#%d (13) {
  ["name"]=>
  string(5) "label"
  ["orgname"]=>
  string(5) "label"
  ["table"]=>
  string(4) "test"
  ["orgtable"]=>
  string(4) "test"
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(%d) "%s"
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(%d)
  ["length"]=>
  int(%d)
  ["charsetnr"]=>
  int(%d)
  ["flags"]=>
  int(0)
  ["type"]=>
  int(254)
  ["decimals"]=>
  int(0)
}
int(2)
mysqli_field_seek(): Argument #2 ($index) must be less than the number of fields for this result set
bool(false)
bool(true)
object(stdClass)#%d (13) {
  ["name"]=>
  string(5) "_null"
  ["orgname"]=>
  string(0) ""
  ["table"]=>
  string(0) ""
  ["orgtable"]=>
  string(0) ""
  ["def"]=>
  string(0) ""
  ["db"]=>
  string(0) ""
  ["catalog"]=>
  string(%d) "%s"
  ["max_length"]=>
  int(0)
  ["length"]=>
  int(0)
  ["charsetnr"]=>
  int(63)
  ["flags"]=>
  int(32896)
  ["type"]=>
  int(6)
  ["decimals"]=>
  int(0)
}
mysqli_result object is already closed
done!

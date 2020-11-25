--TEST--
mysqli_stmt_bind_result() - ZEROFILL
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    function zerofill($offset, $link, $datatype, $insert = 1) {

        mysqli_query($link, 'ALTER TABLE test DROP zero');
        $sql = sprintf('ALTER TABLE test ADD zero %s UNSIGNED ZEROFILL', $datatype);
        if (!mysqli_query($link, $sql)) {
            // no worries - server might not support it
            return true;
        }

        if (!mysqli_query($link, sprintf('UPDATE test SET zero = %s', $insert))) {
            printf("[%03d] UPDATE failed, [%d] %s\n",
                $offset, mysqli_errno($link), mysqli_error($link));
            return false;
        }

        if (!($stmt = mysqli_prepare($link, 'SELECT zero FROM test LIMIT 1'))) {
            printf("[%03d] SELECT failed, [%d] %s\n",
                $offset, mysqli_errno($link), mysqli_error($link));
            return false;
        }

        $result = null;
        if (!mysqli_stmt_bind_result($stmt, $result)) {
            printf("[%03d] Bind failed, [%d] %s\n",
                mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
            return false;
        }

        if (!mysqli_stmt_execute($stmt) || !mysqli_stmt_fetch($stmt)) {
            printf("[%03d] Execute or fetch failed, [%d] %s\n",
                mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
            return false;
        }

        $res = mysqli_stmt_result_metadata($stmt);
        $meta = mysqli_fetch_fields($res);
        mysqli_stmt_free_result($stmt);

        $meta = $meta[0];
        $length = $meta->length;
        if ($length > strlen($insert)) {

            $expected = str_repeat('0', $length - strlen($insert));
            $expected .= $insert;
            if ($expected !== $result) {
                printf("[%03d] Expecting '%s' got '%s'\n", $offset, $expected, $result);
                return false;
            }

        } else if ($length <= 1) {
            printf("[%03d] Length reported is too small to run test\n", $offset);
            return false;
        }


        return true;
    }

    /*
    We map those to PHP numeric types -
        no padding/filling done. Neither with libmysql nor with mysqlnd.
    zerofill(2, $link, 'TINYINT');
    zerofill(3, $link, 'SMALLINT');
    zerofill(4, $link, 'MEDIUMINT');
    zerofill(5, $link, 'INT');
    zerofill(6, $link, 'INTEGER');
    zerofill(7, $link, 'BIGINT');
    zerofill(8, $link, 'FLOAT');
    zerofill(9, $link, 'DOUBLE');
    zerofill(10, $link, 'DOUBLE PRECISION');
    */
    zerofill(11, $link, 'DECIMAL');
    zerofill(12, $link, 'DEC');

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!

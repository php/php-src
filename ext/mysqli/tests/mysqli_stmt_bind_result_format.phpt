--TEST--
Playing with SELECT FORMAT(...) AS _format - see also bugs.php.net/42378
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--INI--
memory_limit=83886080
--FILE--
<?php
    require_once 'connect.inc';

    function create_table($link, $column, $min, $max, $engine, $offset) {

        if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
            printf("[%03d] Cannot drop table test, [%d] %s\n",
                $offset,
                mysqli_errno($link), mysqli_error($link));
            return array();
        }

        $sql = sprintf("CREATE TABLE test(id INT AUTO_INCREMENT PRIMARY KEY, col1 %s) ENGINE=%s",
            $column, $engine);
        if (!mysqli_query($link, $sql)) {
            printf("[%03d] Cannot create table test, [%d] %s\n",
                $offset + 1,
                mysqli_errno($link), mysqli_error($link));
            return array();
        }

        $values = array();
        for ($i = 1; $i <= 100; $i++) {
            $col1 = mt_rand($min, $max);
            $values[$i] = $col1;
            $sql = sprintf("INSERT INTO test(id, col1) VALUES (%d, %f)",
                $i, $col1);
            if (!mysqli_query($link, $sql)) {
                printf("[%03d] Cannot insert data, [%d] %s\n",
                    $offset + 2,
                    mysqli_errno($link), mysqli_error($link));
                return array();
            }
        }

        return $values;
    }

    function test_format($link, $format, $from, $order_by, $expected, $offset) {

        if (!$stmt = mysqli_stmt_init($link)) {
            printf("[%03d] Cannot create PS, [%d] %s\n",
                $offset,
                mysqli_errno($link), mysqli_error($link));
            return false;
        }

        if ($order_by)
            $sql = sprintf('SELECT %s AS _format FROM %s ORDER BY %s', $format, $from, $order_by);
        else
            $sql = sprintf('SELECT %s AS _format FROM %s', $format, $from);

        if (!mysqli_stmt_prepare($stmt, $sql)) {
            printf("[%03d] Cannot prepare PS, [%d] %s\n",
                $offset + 1,
                mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
            return false;
        }

        if (!mysqli_stmt_execute($stmt)) {
            printf("[%03d] Cannot execute PS, [%d] %s\n",
                $offset + 2,
                mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
            return false;
        }

        if (!mysqli_stmt_store_result($stmt)) {
            printf("[%03d] Cannot store result set, [%d] %s\n",
                $offset + 3,
                mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
            return false;
        }

        if (!is_array($expected)) {

            $result = null;
            if (!mysqli_stmt_bind_result($stmt, $result)) {
                printf("[%03d] Cannot bind result, [%d] %s\n",
                    $offset + 4,
                    mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
                return false;
            }

            if (!mysqli_stmt_fetch($stmt)) {
                printf("[%03d] Cannot fetch result,, [%d] %s\n",
                    $offset + 5,
                    mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
                return false;
            }

            if ($result !== $expected) {
                printf("[%03d] Expecting %s/%s got %s/%s with %s - %s.\n",
                    $offset + 6,
                    gettype($expected), $expected,
                    gettype($result), $result,
                    $format, $sql);
            }

        } else {

            $order_by_col = $result = null;
            if (!mysqli_stmt_bind_result($stmt, $order_by_col, $result)) {
                printf("[%03d] Cannot bind result, [%d] %s\n",
                    $offset + 7,
                    mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
                return false;
            }

            foreach ($expected as $k => $v) {
                if (!mysqli_stmt_fetch($stmt)) {
                    break;
                }
                if ($result !== $v) {
                    printf("[%03d] Row %d - expecting %s/%s got %s/%s [%s] with %s - %s.\n",
                        $offset + 8,
                        $k,
                        gettype($v), $v,
                        gettype($result), $result,
                        $order_by_col,
                        $format, $sql);
                }
            }

        }

        mysqli_stmt_free_result($stmt);
        mysqli_stmt_close($stmt);

        return true;
    }

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect - [%d] %s\n",
            mysqli_connect_errno(),
            mysqli_connect_error());

    /* select from dual - pseudo table */
    test_format($link, 'FORMAT(1.01, 0)', 'DUAL', null, '1', 10);
    test_format($link, 'FORMAT(1.23, 1)', 'DUAL', null, '1.2', 20);
    test_format($link, 'FORMAT(1.23, 2)', 'DUAL', null, '1.23', 30);
    test_format($link, 'FORMAT(1234.567, 3)', 'DUAL', null, '1,234.567', 40);
    /* no typo! */
    test_format($link, 'FORMAT(1234.567, 4)', 'DUAL', null, '1,234.5670', 50);

    mysqli_close($link);
    require_once 'table.inc';

    /* select from existing table */
    test_format($link, 'FORMAT(id, 0)', 'test', null, '1', 60);
    test_format($link, 'FORMAT(id + 0.1, 1)', 'test', null, '1.1', 70);
    test_format($link, 'FORMAT(id + 0.01, 2)', 'test', null, '1.01', 80);

    /* create new table and select from it */
    $expected = create_table($link, 'FLOAT', -10000, 10000, $engine, 90);
    foreach ($expected as $k => $v)
        $expected[$k] = number_format(round($v), 0, '.', ',');
    test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 100);

    $expected = create_table($link, 'FLOAT UNSIGNED', 0, 10000, $engine, 110);
    foreach ($expected as $k => $v)
        $expected[$k] = number_format(round($v), 0, '.', ',');
    test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 120);

    $expected = create_table($link, 'TINYINT', -128, 127, $engine, 130);
    foreach ($expected as $k => $v)
        $expected[$k] = number_format(round($v), 0, '.', ',');
    test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 140);

    $expected = create_table($link, 'SMALLINT UNSIGNED', 0, 65535, $engine, 150);
    foreach ($expected as $k => $v)
        $expected[$k] = number_format(round($v), 0, '.', ',');
    test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 160);

    $expected = create_table($link, 'MEDIUMINT', 0, 8388607, $engine, 170);
    foreach ($expected as $k => $v)
        $expected[$k] = number_format(round($v), 0, '.', ',');
    test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 180);

    $expected = create_table($link, 'INT UNSIGNED', 0, 1000, $engine, 190);
    foreach ($expected as $k => $v)
        $expected[$k] = number_format(round($v), 0, '.', ',');
    test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 200);

    $expected = create_table($link, 'BIGINT', -1000, 1000, $engine, 210);
    foreach ($expected as $k => $v)
        $expected[$k] = number_format(round($v), 0, '.', ',');
    test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 220);

    $expected = create_table($link, 'DECIMAL(5,0)', -1000, 1000, $engine, 230);
    foreach ($expected as $k => $v)
        $expected[$k] = number_format(round($v), 0, '.', ',');
    test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 240);

    // http://bugs.php.net/bug.php?id=42378
    if (!mysqli_query($link, "DROP TABLE IF EXISTS test")) {
        printf("[300] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    if (mysqli_query($link, "CREATE TABLE `test` (
  `targetport` int(11) NOT NULL default '0',
  `sources` double(17,4) default NULL,
  `current_sources` double(17,4) default NULL,
  `reports` double(17,4) default NULL,
  `current_reports` double(17,4) default NULL,
  `targets` double(17,4) default NULL,
  `current_targets` double(17,4) default NULL,
  `maxsources` int(11) default NULL,
  `maxtargets` int(11) default NULL,
  `maxreports` int(11) default NULL,
  `trend` float default NULL,
  PRIMARY KEY  (`targetport`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1")) {

        do {
            $values = array();
            for ($i = 0; $i < 200; $i++) {
                $current_targets = mt_rand(-100000, 100000) / 10;
                do {
                    $trend = (mt_rand(0, 3) > 1) ? (mt_rand(-10000, 10000) / 100) : 'NULL';
                } while (isset($values[(string)$trend]));

                $sql = sprintf('INSERT INTO test(targetport, current_targets, maxreports, trend) VALUES (%d, %f, %s, %s)',
                    $i,
                    $current_targets,
                    (mt_rand(0, 1) > 0) ? mt_rand(0, 1000) : 'NULL',
                    $trend);
                if (!mysqli_query($link, $sql)) {
                    printf("[301] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
                    break 2;
                }
                if ($current_targets > 0 && $trend !== 'NULL')
                    $values[(string)$trend] = $i;
            }
            krsort($values);

            if (!$stmt = mysqli_stmt_init($link)) {
                printf("[302] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
                break;
            }

            if (!mysqli_stmt_prepare($stmt, 'SELECT trend, targetport, FORMAT(trend, 2) FROM test WHERE current_targets > 0 AND trend IS NOT NULL ORDER BY trend DESC LIMIT 100')) {
                printf("[303] [%d] %s\n", mysqli_stmt_errno($link), mysqli_stmt_error($link));
                break;
            }

            if (!mysqli_stmt_execute($stmt)) {
                printf("[304] [%d] %s\n", mysqli_stmt_errno($link), mysqli_stmt_error($link));
                break;
            }

            if (!mysqli_stmt_store_result($stmt)) {
                printf("[305] [%d] %s\n", mysqli_stmt_errno($link), mysqli_stmt_error($link));
                break;
            }

            $trend = $targetport = $format = null;
            if (!mysqli_stmt_bind_result($stmt, $trend, $targetport, $format)) {

                printf("[305] [%d] %s\n", mysqli_stmt_errno($link), mysqli_stmt_error($link));
                break;
            }

            foreach ($values as $exp_trend => $exp_targetport) {
                if (!mysqli_stmt_fetch($stmt)) {
                    break;
                }
                if ($targetport != $exp_targetport) {
                    printf("[306] Values fetched from MySQL seem to be wrong, check manually\n");
                    printf("%s/%s - %s/%s - '%s'\n", $trend, $exp_trend, $targetport, $exp_targetport, $format);
                }
            }
            mysqli_stmt_free_result($stmt);
            mysqli_stmt_close($stmt);

            // same but OO interface
            if (!$stmt = mysqli_stmt_init($link)) {
                printf("[307] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
                break;
            }

            if (!$stmt->prepare('SELECT trend, targetport, FORMAT(trend, 2) FROM test WHERE current_targets > 0 AND trend IS NOT NULL ORDER BY trend DESC LIMIT 100')) {
                printf("[308] [%d] %s\n", mysqli_stmt_errno($link), mysqli_stmt_error($link));
                break;
            }

            if (!$stmt->execute()) {
                printf("[309] [%d] %s\n", mysqli_stmt_errno($link), mysqli_stmt_error($link));
                break;
            }

            if (!$stmt->store_result()) {
                printf("[310] [%d] %s\n", mysqli_stmt_errno($link), mysqli_stmt_error($link));
                break;
            }

            $trend = $targetport = $format = null;
            if (!$stmt->bind_result($trend, $targetport, $format)) {

                printf("[311] [%d] %s\n", mysqli_stmt_errno($link), mysqli_stmt_error($link));
                break;
            }

            foreach ($values as $exp_trend => $exp_targetport) {
                if (!$stmt->fetch()) {
                    break;
                }
                if ($targetport != $exp_targetport) {
                    printf("[312] Values fetched from MySQL seem to be wrong, check manually\n");
                    printf("%s/%s - %s/%s - '%s'\n", $trend, $exp_trend, $targetport, $exp_targetport, $format);
                }
            }
            $stmt->free_result();
            $stmt->close();

        } while (false);

    } else {
        var_dump(mysqli_error($link));
    }


    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
	require_once 'clean_table.inc';
?>
--EXPECT--
done!

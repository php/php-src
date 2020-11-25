--TEST--
mysqli_stmt_bind_result()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    function dec32bin($dec, $bits) {

        $maxval = pow(2, $bits);
        $bin = '';
        for ($bitval = $maxval; $bitval >= 1; $bitval = $bitval / 2) {
            if (($dec / $bitval) >= 1) {
                $bin .= '1';
                $dec -= $bitval;
            } else {
                $bin .= '0';
            }
        }
        return $bin;
    }

    if (!$link_ins = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!$link_sel = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    for ($bits = 1; $bits <= 46; $bits++) {

        if (1 == $bits)
            $max_value = 1;
        else
            $max_value = pow(2, $bits) - 1;

        if (!mysqli_query($link_ins, "DROP TABLE IF EXISTS test")) {
            printf("[003 - %d] [%d] %s\n", $bits, mysqli_errno($link_ins), mysqli_error($link_ins));
        }

        if (!mysqli_query($link_ins, sprintf("CREATE TABLE test(id BIGINT, bit_value BIT(%d) NOT NULL, bit_null BIT(%d) DEFAULT NULL) ENGINE = %s", $bits, $bits, $engine))) {
            // don't bail - column type might not be supported by the server, ignore this
            continue;
        }
        if (!$stmt_ins = mysqli_stmt_init($link_ins)) {
            printf("[004 - %d] [%d] %s\n", $bits, mysqli_errno($link_ins), mysqli_error($link_ins));
            continue;
        }

        if (!mysqli_stmt_prepare($stmt_ins, "INSERT INTO test(id, bit_value) VALUES (?, ?)")) {
            printf("[005 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt_ins), mysqli_stmt_error($stmt_ins));
            mysqli_stmt_close($stmt_ins);
            continue;
        }

        if (!($stmt_sel = mysqli_stmt_init($link_sel))) {
            printf("[006 - %d] [%d] %s\n", $bits, mysqli_errno($link_sel), mysqli_error($link_sel));
            mysqli_stmt_close($stmt_ins);
                continue;
        }

        $tests = 0;
        $rand_max = mt_getrandmax();
        while ($tests < 10) {

            $tests++;
            if (1 == $tests)
                $value = 0;
            else if (2 == $tests)
                $value = $max_value;
            else {
                if ($max_value > $rand_max) {
                    $max_loops = floor($max_value/$rand_max);
                    $num_loops = mt_rand(1, $max_loops);
                    $value = 0;
                    for ($i = 0; $i < $num_loops; $i++)
                        $value += mt_rand(0, $rand_max);
                } else {
                    $value = mt_rand(0, $max_value);
                }
            }

            $bin = ($bits < 32) ? decbin($value) : dec32bin($value, $bits);
            for ($i = 0; ($i < strlen($bin)) && ($bin[$i] == '0'); $i++)
                ;
            $bin2 = substr($bin, $i, strlen($bin));

            if (!mysqli_stmt_bind_param($stmt_ins, "dd", $value, $value)) {
                printf("[007 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt_ins), mysqli_stmt_error($stmt_ins));
                mysqli_stmt_close($stmt_ins);
                continue;
            }
            if (!mysqli_stmt_execute($stmt_ins)) {
                printf("[008 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt_ins), mysqli_stmt_error($stmt_ins));
                break;
            }
            $sql = sprintf("SELECT id, BIN(bit_value) AS _bin, bit_value, bit_value + 0 AS _bit_value0, bit_null FROM test WHERE id = %s", $value);
            if ((!mysqli_stmt_prepare($stmt_sel, $sql)) ||
                (!mysqli_stmt_execute($stmt_sel))) {
                printf("[009 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt_sel), mysqli_stmt_error($stmt_sel));
                break;
            }

            $row = array('id' => -1, '_bin' => - 1, 'bit_value' => -1, '_bit_value0' => -1, 'bit_null' => -1);
            if (!mysqli_stmt_bind_result($stmt_sel, $row['id'], $row['_bin'], $row['bit_value'], $row['_bit_value0'], $row['bit_null'])) {
                printf("[010 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt_sel), mysqli_stmt_error($stmt_sel));
                break;
            }

            if (!($ret = mysqli_stmt_fetch($stmt_sel))) {
                printf("[011 - %d] mysqli_stmt_fetch() has failed for %d bits - ret = %s/%s, [%d] %s, [%d] %s\n",
                    $bits, $bits,
                    gettype($ret), $ret,
                    mysqli_stmt_errno($stmt_sel), mysqli_stmt_error($stmt_sel),
                    mysqli_errno($link_sel), mysqli_errno($link_sel)
                );
                break;
            }

            if (($value != $row['id']) || (($bin != $row['_bin']) && ($bin2 != $row['_bin']))) {
                debug_zval_dump($row);
                printf("[012 - %d] Insert of %s in BIT(%d) column might have failed. id = %s, bin = %s (%s/%s)\n",
                    $bits, $value, $bits, $row['id'], $row['_bin'], $bin, $bin2);
                break;
            }
            if ($value != $row['bit_value']) {
                debug_zval_dump($row);
                printf("[013 - %d] Expecting %s got %s\n", $bits, $value, $row['bit_value']);
                break;
            }

            if (null !== $row['bit_null']) {
                debug_zval_dump($row);
                printf("[014 - %d] Expecting null got %s/%s\n", $bits, gettype($row['bit_value']), $row['bit_value']);
                break;
            }

        }

        mysqli_stmt_close($stmt_ins);
        mysqli_stmt_close($stmt_sel);
    }

    mysqli_close($link_ins);
    mysqli_close($link_sel);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!

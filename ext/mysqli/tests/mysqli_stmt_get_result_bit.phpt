--TEST--
Fetching BIT column values using the PS API
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
if (!function_exists('mysqli_stmt_get_result')) {
    die("skip mysqli_stmt_get_result() not available");
}

require_once 'connect.inc';
if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
}

if (mysqli_get_server_version($link) < 50003) {
    // b'001' syntax not supported before 5.0.3
    die("skip Syntax used for test not supported with MySQL Server before 5.0.3");
}
?>
--FILE--
<?php
    require_once 'connect.inc';

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

    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    for ($bits = 1; $bits <= 46; $bits++) {
        if (1 == $bits)
            $max_value = 1;
        else
            $max_value = pow(2, $bits) - 1;

        if (!mysqli_query($link, "DROP TABLE IF EXISTS test") ||
            !mysqli_query($link, $sql = sprintf('CREATE TABLE test(id BIGINT UNSIGNED, bit_value BIT(%d) NOT NULL, bit_null BIT(%d) DEFAULT NULL) ENGINE="%s"', $bits, $bits, $engine)))
            printf("[002 - %d] [%d] %s\n",$bits, mysqli_errno($link), mysqli_error($link));

        if (!$stmt = mysqli_stmt_init($link))
            printf("[003 - %d] [%d] %s\n", $bits, mysqli_errno($link), mysqli_error($link));

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
            $sql = sprintf("INSERT INTO test(id, bit_value) VALUES (%s, b'%s')", $value, $bin);
            for ($i = 0; ($i < strlen($bin)) && ($bin[$i] == '0'); $i++)
                ;
            $bin2 = substr($bin, $i, strlen($bin));

            if (!mysqli_stmt_prepare($stmt, $sql) ||
                    !mysqli_stmt_execute($stmt))
                printf("[004 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

            $sql = sprintf("SELECT bin(bit_value) AS _bin, id, bit_value, bit_null FROM test WHERE id = %s", $value);
            if (!mysqli_stmt_prepare($stmt, $sql) ||
                    !mysqli_stmt_execute($stmt))
                printf("[005 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

            if (!$res = mysqli_stmt_get_result($stmt))
                printf("[006 - %d] [%d] %s\n", $bits, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

            if (!$row = mysqli_fetch_assoc($res))
                printf("[007 - %d] [%d] %s\n", $bits, mysqli_errno($link), mysqli_error($link));
            mysqli_free_result($res);

            if (($value != $row['id']) || (($bin != $row['_bin']) && ($bin2 != $row['_bin']))) {
                debug_zval_dump($row);
                printf("[008 - %d] Insert of %s in BIT(%d) column might have failed. id = %s, bin = %s (%s/%s)\n",
                    $bits, $value, $bits, $row['id'], $row['_bin'], $bin, $bin2);
                break;
            }
            if ($value != $row['bit_value']) {
                debug_zval_dump($row);
                printf("%10s %64s\n%10s %64s\n", '_bin', $row['_bin'], 'insert', $bin);
                printf("[009 - %d] Expecting %s got %s\n", $bits, $value, $row['bit_value']);
                break;
            }

            if (null !== $row['bit_null']) {
                debug_zval_dump($row);
                printf("[010 - %d] Expecting null got %s/%s\n", $bits, gettype($row['bit_value']), $row['bit_value']);
                break;
            }

        }

        mysqli_stmt_close($stmt);

    }

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
require_once "clean_table.inc";
?>
--EXPECT--
done!

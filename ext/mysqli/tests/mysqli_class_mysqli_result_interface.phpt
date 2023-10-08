--TEST--
Interface of the class mysqli_result
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    $mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
    $mysqli_result = $mysqli->query('SELECT * FROM test');
    $row = $mysqli_result->fetch_row();

    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
    $res = mysqli_query($link, 'SELECT * FROM test');
    assert(mysqli_fetch_row($res) === $row);

    printf("Parent class:\n");
    var_dump(get_parent_class($mysqli_result));

    printf("\nMethods:\n");
    $methods = get_class_methods($mysqli_result);
    $expected_methods = array(
        '__construct'           => true,
        'close'                 => true,
        'data_seek'             => true,
        'fetch_all'             => true,
        'fetch_array'           => true,
        'fetch_assoc'           => true,
        'fetch_field'           => true,
        'fetch_field_direct'    => true,
        'fetch_fields'          => true,
        'fetch_object'          => true,
        'fetch_row'             => true,
        'fetch_column'          => true,
        'field_seek'            => true,
        'free'                  => true,
        'free_result'           => true,
        'getIterator'           => true,
    );

    foreach ($methods as $k => $method) {
        if (isset($expected_methods[$method])) {
            unset($expected_methods[$method]);
            unset($methods[$k]);
        }
        if ($method == 'mysqli_result') {
            // get_class_method reports different constructor names
            unset($expected_methods['__construct']);
            unset($methods[$k]);
        }
    }

    if (!empty($expected_methods)) {
        printf("Dumping list of missing methods.\n");
        var_dump($expected_methods);
    }
    if (!empty($methods)) {
        printf("Dumping list of unexpected methods.\n");
        var_dump($methods);
    }
    if (empty($expected_methods) && empty($methods))
        printf("ok\n");


    printf("\nClass variables:\n");
    $variables = array_keys(get_class_vars(get_class($mysqli_result)));
    sort($variables);
    foreach ($variables as $k => $var)
        printf("%s\n", $var);

    printf("\nObject variables:\n");
    $variables = array_keys(get_object_vars($mysqli_result));
    foreach ($variables as $k => $var)
        printf("%s\n", $var);

    printf("\nMagic, magic properties:\n");

    assert(($tmp = mysqli_field_tell($res)) === $mysqli_result->current_field);
    printf("mysqli_result->current_field = '%s'/%s ('%s'/%s)\n",
        $mysqli_result->current_field, gettype($mysqli_result->current_field),
        $tmp, gettype($tmp));

    assert(($tmp = mysqli_field_count($link)) === $mysqli_result->field_count);
    printf("mysqli_result->field_count = '%s'/%s ('%s'/%s)\n",
        $mysqli_result->field_count, gettype($mysqli_result->field_count),
        $tmp, gettype($tmp));

    assert(($tmp = mysqli_fetch_lengths($res)) === $mysqli_result->lengths);
    printf("mysqli_result->lengths -> '%s'/%s ('%s'/%s)\n",
        ((is_array($mysqli_result->lengths)) ? implode(' ', $mysqli_result->lengths) : 'n/a'),
        gettype($mysqli_result->lengths),
        ((is_array($tmp)) ? implode(' ', $tmp) : 'n/a'),
        gettype($tmp));

    assert(($tmp = mysqli_num_rows($res)) === $mysqli_result->num_rows);
    printf("mysqli_result->num_rows = '%s'/%s ('%s'/%s)\n",
        $mysqli_result->num_rows, gettype($mysqli_result->num_rows),
        $tmp, gettype($tmp));

    assert(in_array($mysqli_result->type, array(MYSQLI_STORE_RESULT, MYSQLI_USE_RESULT)));
    printf("mysqli_result->type = '%s'/%s\n",
        ((MYSQLI_STORE_RESULT == $mysqli_result->type) ? 'store' : 'use'),
        gettype($mysqli_result->type));

    printf("\nAccess to undefined properties:\n");
    printf("mysqli_result->unknown = '%s'\n", @$mysqli_result->unknown);

    printf("\nConstructor:\n");
    $res = new mysqli_result($link);

    try {
        $res->num_rows;
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    if (!mysqli_query($link, "SELECT id FROM test ORDER BY id"))
        printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    $res = new mysqli_result($link);
    $res = new mysqli_result($link, MYSQLI_STORE_RESULT);
    $res = new mysqli_result($link, MYSQLI_USE_RESULT);

    $valid = array(MYSQLI_STORE_RESULT, MYSQLI_USE_RESULT);
    do {
        $mode = mt_rand(-1000, 1000);
    } while (in_array($mode, $valid));

    if ($TEST_EXPERIMENTAL) {
        ob_start();
        $res = new mysqli_result($link, $mode);
        $content = ob_get_contents();
        ob_end_clean();
        if (!stristr($content, 'Invalid value for resultmode'))
            printf("[009] Expecting warning because of invalid resultmode\n");
    }

    print "done!";
?>
--EXPECT--
Parent class:
bool(false)

Methods:
ok

Class variables:
current_field
field_count
lengths
num_rows
type

Object variables:

Magic, magic properties:
mysqli_result->current_field = '0'/integer ('0'/integer)
mysqli_result->field_count = '2'/integer ('2'/integer)
mysqli_result->lengths -> '1 1'/array ('1 1'/array)
mysqli_result->num_rows = '6'/integer ('6'/integer)
mysqli_result->type = 'store'/integer

Access to undefined properties:
mysqli_result->unknown = ''

Constructor:
mysqli_result object is already closed
done!

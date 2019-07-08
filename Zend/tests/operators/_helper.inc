<?php

function get_test_values() {
    return [
        false,
        true,
        0,
        10,
        0.0,
        10.0,
        3.14,
        '0',
        '10',
        '010',
        '10 elephants',
        'foo',
        [],
        [1],
        [1, 100],
        ['foo' => 1, 'bar' => 2],
        ['bar' => 1, 'foo' => 2],
        (object)[],
        (object)['foo' => 1, 'bar' => 2],
        (object)['bar' => 1, 'foo' => 2],
        new DateTime(),
        fopen('php://temp', 'r+'),
        null,
    ];
}

function var_out($value) {
    if (is_resource($value)) {
        return 'resource';
    }
    if ($value instanceof DateTime) {
        return 'DateTime';
    }
    if ($value instanceof stdClass) {
        $pre = '(object) ';
        $value = (array)$value;
    }
    return ($pre ?? '') . preg_replace(['/\n\s*/', '/, \)/'], [' ', ' )'], var_export($value, true));
}

function var_out_base64($value) {
    if (is_string($value)) {
        return 'base64:' . base64_encode($value);
    }
    return var_out($value);
};

function error_to_exception($errno, $errstr, $errfile, $errline) {
    if ($errno === E_RECOVERABLE_ERROR) {
        throw new ErrorException($errstr, 0, $errno);
    }
    return false;
}

function err_out($err) {
    $errTypes = [E_RECOVERABLE_ERROR => 'Catchable error', E_WARNING => 'Warning', E_NOTICE => 'Notice'];
    return $err !== null ? ' - ' . $errTypes[$err['type']] . ' ' . $err['message'] : '';
}

function one_operand($statement, $fn, $a, $var_out = 'var_out') {
    error_clear_last();
    echo strtr($statement, ['$a' => var_out($a)]);

    try {
        $res = @$fn($a);
    } catch (ErrorException $e) {
        echo err_out(['type' => $e->getSeverity(), 'message' => $e->getMessage()]), "\n";
    } catch (Throwable $e) {
        echo ' - ', get_class($e), ' ', $e->getMessage(), "\n";
        return;
    }

    $err = error_get_last();
    echo ' = ', $var_out($res, $a), err_out($err), "\n";
}

function test_one_operand($statement, $fn, $var_out = 'var_out') {
    $values = get_test_values();

    foreach ($values as $a) {
        one_operand($statement, $fn, $a, $var_out);
    }
}

function two_operands($statement, $fn, $a, $b, $var_out = 'var_out') {
    error_clear_last();
    echo strtr($statement, ['$a' => var_out($a), '$b' => var_out($b)]);

    try {
        $res = @$fn($a, $b);
    } catch (ErrorException $e) {
        echo err_out(['type' => $e->getSeverity(), 'message' => $e->getMessage()]), "\n";
    } catch (Throwable $e) {
        echo ' - ', get_class($e), ' ', $e->getMessage(), "\n";
        return;
    }

    $err = error_get_last();
    echo ' = ', $var_out($res, $a, $b), err_out($err), "\n";
}

function test_two_operands($statement, $fn, $var_out = 'var_out') {
    $values = get_test_values();

    foreach ($values as $a) {
        foreach ($values as $b) {
            two_operands($statement, $fn, $a, $b, $var_out);
        }
    }
}

--TEST--
SCCP 025: ADD_ARRAY_ELEMENT against an existing const array
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test($phpEx)
{
    $expected_data_sets = array(
        1 => array(
            'id'  => 1,
        ),
        6 => array(
            'viewtopic' => strval("phpBB/viewtopic.$phpEx"),

        ),
    );

    $test_cases = array(
        array(
            'expected' => array(6),
        ),
    );

    foreach ($test_cases as $case => $case_data)
    {
        foreach ($case_data['expected'] as $data_set => $expected)
        {
            $test_cases[$case]['expected'][$data_set] = $expected_data_sets[$expected];
        }
    }

    return $test_cases;
}
var_dump(test("xxx"));
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["expected"]=>
    array(1) {
      [0]=>
      array(1) {
        ["viewtopic"]=>
        string(19) "phpBB/viewtopic.xxx"
      }
    }
  }
}

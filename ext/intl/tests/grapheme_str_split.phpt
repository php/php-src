--TEST--
grapheme_str_split function tests
--EXTENSIONS--
intl
--FILE--
<?php
function ut_main()
{
    $res_str = '';

    $tests = array(
        array( "abc", 3, array("abc") ),
        array( "abc", 2, array("ab", "c") ),
        array( "abc", 1, array("a", "b", "c" ) ),
        array( "土下座🙇‍♀を", 1, array("土", "下", "座", "🙇‍♀", "を") ),
        array( "土下座🙇‍♀を", 6, array("土下座🙇‍♀を") ),
    );

    foreach( $tests as $test ) {
        $res_str .= "grapheme cluster for str_split - param {$test[0]}, length {$test[1]} ";
        $result = grapheme_str_split($test[0], $test[1]);
        $res_str .= json_encode($test[count($test)-1]) . check_result($result, $test[count($test)-1]) . "\n";
    }
    return $res_str;
}

echo ut_main();

function check_result($result, $expected) {

    if ( $result === false ) {
        $result = 'false';
    }

    if ( $result !== $expected) {
        echo "result: {$result}\n";
        echo "expected: {$expected}\n";
    }

    return "";
}
?>
--EXPECT--
grapheme cluster for str_split - param abc, length 3 ["abc"]
grapheme cluster for str_split - param abc, length 2 ["ab","c"]
grapheme cluster for str_split - param abc, length 1 ["a","b","c"]
grapheme cluster for str_split - param 土下座🙇‍♀を, length 1 ["\u571f","\u4e0b","\u5ea7","\ud83d\ude47\u200d\u2640","\u3092"]
grapheme cluster for str_split - param 土下座🙇‍♀を, length 6 ["\u571f\u4e0b\u5ea7\ud83d\ude47\u200d\u2640\u3092"]

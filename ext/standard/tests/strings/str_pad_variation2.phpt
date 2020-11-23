--TEST--
str_pad() function: usage variations - Non printable chars
--INI--
precision=14
--FILE--
<?php
// Split from str_pad for NUL Bytes
// 7-bit ASCII
$string = chr(0).chr(255).chr(128).chr(234).chr(143);

/* different pad_lengths */
$pad_lengths = [
    -PHP_INT_MAX,  // huge negative value
    -1,  // negative value
    0,  // pad_length < sizeof(input_string)
    9,  // pad_length <= sizeof(input_string)
    10,  // pad_length > sizeof(input_string)
    16,  // pad_length > sizeof(input_string)
];

$pad_string = "=";

/*loop through to use each variant of $pad_length on
  each element of $input_strings array */
foreach ($pad_lengths as $pad_length ) {
    // default pad_string & pad_type
    var_dump( bin2hex( str_pad($string, $pad_length) ) );
    // default pad_type
    var_dump( bin2hex( str_pad($string, $pad_length, $pad_string) ) );
    var_dump( bin2hex( str_pad($string, $pad_length, $pad_string, STR_PAD_LEFT) ) );
    var_dump( bin2hex( str_pad($string, $pad_length, $pad_string, STR_PAD_RIGHT) ) );
    var_dump( bin2hex( str_pad($string, $pad_length, $pad_string, STR_PAD_BOTH) ) );
}

?>

DONE
--EXPECT--
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(10) "00ff80ea8f"
string(18) "00ff80ea8f20202020"
string(18) "00ff80ea8f3d3d3d3d"
string(18) "3d3d3d3d00ff80ea8f"
string(18) "00ff80ea8f3d3d3d3d"
string(18) "3d3d00ff80ea8f3d3d"
string(20) "00ff80ea8f2020202020"
string(20) "00ff80ea8f3d3d3d3d3d"
string(20) "3d3d3d3d3d00ff80ea8f"
string(20) "00ff80ea8f3d3d3d3d3d"
string(20) "3d3d00ff80ea8f3d3d3d"
string(32) "00ff80ea8f2020202020202020202020"
string(32) "00ff80ea8f3d3d3d3d3d3d3d3d3d3d3d"
string(32) "3d3d3d3d3d3d3d3d3d3d3d00ff80ea8f"
string(32) "00ff80ea8f3d3d3d3d3d3d3d3d3d3d3d"
string(32) "3d3d3d3d3d00ff80ea8f3d3d3d3d3d3d"

DONE

--TEST--
strcasecmp() function
--INI--
precision=12
--FILE--
<?php
/* Compares two strings in case-insensitive manner */

/* creating an array of strings to be compared */
$strs = [chr(125), chr(255)];

foreach ($strs as $str1) {
    foreach ($strs as $str2) {
        var_dump(strcasecmp($str1, $str2));
    }
}

?>
--EXPECT--
int(0)
int(-130)
int(130)
int(0)

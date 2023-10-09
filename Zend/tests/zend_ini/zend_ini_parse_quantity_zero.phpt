--TEST--
Test parsing of valid 0 quantities
--EXTENSIONS--
zend_test
--FILE--
<?php

$tests = [
    '0',
    '0K',
    '0k',
    '0M',
    '0m',
    '0G',
    '0g',
    '-0',
    '-0K',
    '-0k',
    '-0M',
    '-0m',
    '-0G',
    '-0g',
];

foreach ($tests as $setting) {
    printf("# \"%s\"\n", addcslashes($setting, "\0..\37!@\177..\377"));
    var_dump(zend_test_zend_ini_parse_quantity($setting));
    print "\n";
}
--EXPECT--
# "0"
int(0)

# "0K"
int(0)

# "0k"
int(0)

# "0M"
int(0)

# "0m"
int(0)

# "0G"
int(0)

# "0g"
int(0)

# "-0"
int(0)

# "-0K"
int(0)

# "-0k"
int(0)

# "-0M"
int(0)

# "-0m"
int(0)

# "-0G"
int(0)

# "-0g"
int(0)

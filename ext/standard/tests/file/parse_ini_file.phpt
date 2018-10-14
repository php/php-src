--TEST--
Test parse_ini_file() function
--FILE--
<?php
/* Prototype: array parse_ini_file(string $filename [,bool $process_sections]);
   Description: parse_ini_file() loads in the ini file specified in filename,
     and returns the settings in it in an associative array.
*/

$file_path = dirname(__FILE__);

$parse_string = <<<EOD
; Comment starts with semi-colon(;)
; Section starts with [<section name>]

; start of ini file

[Constans]
one = 1
five = 5
animal = BIRD
Language = PHP
PHP_CONSTANT = 1.2345678
10 = Ten
HELLO = HELLO

[date]
date = 
time =

[paths]
path = /usr/local/bin
URL = http://www.php.net

[Decimal]
Decimal_value1 = 100
Decimal_value2 = -100
Decimal_value3 = -2147483647
Decimal_value4 = 2147483647
Decimal_value5 = -2147483648
Decimal_value6 = 2147483648

[Octal]
Octal_value = 0100

[Hex]
Hex_value1 = 0x101
Hex_Value2 = 0x102
Hex_Value2 = 0x103

[Non-alphanumerics_as_values]
;Non-alpha numeric chars without quotes
Non_alpha1 = ;
Non_alpha2 = +
Non_alpha3 = *
Non_alpha4 = %
Non_alpha5 = <>
Non_alpha6 = @
Non_alpha7 = #
Non_alpha8 = -
Non_alpha9 = :
Non_alpha10 = ?
Non_alpha11 = /
Non_alpha12 = \
;These chars have a special meaning when used in the value,
;  hence parser throws an error
;Non_alpha13 = & 
;Non_alpha14 = ^
;Non_alpha15 = {}
;Non_alpha16 = |
;Non_alpha17 = ~
;Non_alpha18 = !
;Non_alpha19 = $
;Non_alpha20 = () 

Non_alpha1_quotes = ";"
Non_alpha2_quotes = "+"
Non_alpha3_quotes = "*"
Non_alpha4_quotes = "%"
Non_alpha5_quotes = "<>"
Non_alpha6_quotes = "@"
Non_alpha7_quotes = "#"
Non_alpha8_quotes = "^"
Non_alpha9_quotes = "-"
Non_alpha10_quotes = "="
Non_alpha11_quotes = ":"
Non_alpha12_quotes = "?"
Non_alpha13_quotes = "/"
;Non_alpha14_quotes = "\"
Non_alpha15_quotes = "&"
Non_alpha16_quotes = "{}"
Non_alpha17_quotes = "|"
Non_alpha18_quotes = "~"
Non_alpha19_quotes = "!"
;Non_alpha20_quotes = "$"
Non_alpha21_quotes = "()"

[Non-alpha numerics in strings]
;expected error, as the non-alphanumeric chars not enclosed in double quotes("")
Non_alpha_string1 = Hello@world
;Non_alpha_string2 = Hello!world
;Non_alpha_string3 = Hello#world
;Non_alpha_string4 = Hello%world
;Non_alpha_string5 = Hello&world
;Non_alpha_string6 = Hello*world
;Non_alpha_string7 = Hello+world
;Non_alpha_string8 = Hello-world
;Non_alpha_string9 = Hello'world
;Non_alpha_string10 = Hello:world
;Non_alpha_string11 = Hello;world
;Non_alpha_string12 = Hello<world
;Non_alpha_string13 = Hello>world
;Non_alpha_string14 = Hello>world
;Non_alpha_string15 = Hello?world
;Non_alpha_string16 = Hello\world
;Non_alpha_string17 = Hello^world
;Non_alpha_string18 = Hello_world
;Non_alpha_string19 = Hello|world
;Non_alpha_string20 = Hello~world
;Non_alpha_string21 = Hello`world
;Non_alpha_string22 = Hello(world)

[Non-alpha numerics in strings -with quotes]
Non_alpha_string1_quotes = "Hello@world"
Non_alpha_string2_quotes = "Hello!world"
Non_alpha_string3_quotes = "Hello#world"
Non_alpha_string4_quotes = "Hello&world"
Non_alpha_string5_quotes = "Hello*world"
Non_alpha_string6_quotes = "Hello+world"
Non_alpha_string7_quotes = "Hello-world"
Non_alpha_string8_quotes = "Hello'world"
Non_alpha_string9_quotes = "Hello:world"
Non_alpha_string10_quotes = "Hello;world"
Non_alpha_string11_quotes = "Hello<world"
Non_alpha_string12_quotes = "Hello>world"
Non_alpha_string13_quotes = "Hello>world"
Non_alpha_string14_quotes = "Hello?world"
Non_alpha_string15_quotes = "Hello\world"
Non_alpha_string16_quotes = "Hello^world"
Non_alpha_string17_quotes = "Hello_world"
Non_alpha_string18_quotes = "Hello|world"
Non_alpha_string19_quotes = "Hello~world"
Non_alpha_string20_quotes = "Hello`world"
Non_alpha_string21_quotes = "Hello(world)"

[Newlines_in_Values]
String1 = "Hello, world\nGood Morning"
String2 = "\nHello, world
             Good Morning\n"
String3 = 'Hello, world\tGood Morning'
String4 = "\n"
String5 = "\n\n"
String6 = Hello, world\tGood Morning

[ReservedKeys_as_Values]
Key1 = YES
Key2 = Yes
Key3 = yEs
Key4 = NO
Key5 = No
Key6 = nO
Key7 = TRUE
Key8 = True
Key9 = tRUE
Key10 = true
Key11 = FALSE
Key12 = False
Key13 = false
Key14 = fAlSE
Key15 = NULL
Key16 = Null
Key17 = nuLL
Key18 = null

[ReservedKeys_as_Keys] 
; Expected:error, reserved key words must not be used as keys for ini file
;YES = 1
;Yes = 2
;yEs = 1.2
;YES = YES
;NO = ""
;No = "string"
;nO = "\0"
;TRUE = 1.1
;True = 1
;tRUE = 5
;true = TRUE
;FALSE = FALSE
;False = ""
;false = "hello"
;fAlSE = ""
;NULL = ""
;Null = 0
;nuLL = "\0"
;null = NULL

; end of ini file
EOD;
/* creating parse.ini file */
$file_handle = fopen($file_path."/parse.ini", "w");
fwrite($file_handle, $parse_string);
fclose($file_handle);

echo "*** Test parse_ini_file() function:  with various keys and values given in parse.ini file ***\n";
echo "-- ini file without process_sections optional arg --\n";
define('BIRD', 'Humming bird');
$ini_array = parse_ini_file($file_path."/parse.ini");
print_r($ini_array);

echo "\n-- ini file with process_sections as TRUE --\n";
$ini_array = parse_ini_file($file_path."/parse.ini", TRUE);
print_r($ini_array);

echo "*** Done **\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/parse.ini");
?>
--EXPECTF--
*** Test parse_ini_file() function:  with various keys and values given in parse.ini file ***
-- ini file without process_sections optional arg --
Array
(
    [one] => 1
    [five] => 5
    [animal] => Humming bird
    [Language] => PHP
    [PHP_CONSTANT] => 1.2345678
    [10] => Ten
    [HELLO] => HELLO
    [date] => 
    [time] => 
    [path] => /usr/local/bin
    [URL] => http://www.php.net
    [Decimal_value1] => 100
    [Decimal_value2] => -100
    [Decimal_value3] => -2147483647
    [Decimal_value4] => 2147483647
    [Decimal_value5] => -2147483648
    [Decimal_value6] => 2147483648
    [Octal_value] => 0100
    [Hex_value1] => 0x101
    [Hex_Value2] => 0x103
    [Non_alpha1] => 
    [Non_alpha2] => +
    [Non_alpha3] => *
    [Non_alpha4] => %
    [Non_alpha5] => <>
    [Non_alpha6] => @
    [Non_alpha7] => #
    [Non_alpha8] => -
    [Non_alpha9] => :
    [Non_alpha10] => ?
    [Non_alpha11] => /
    [Non_alpha12] => \
    [Non_alpha1_quotes] => ;
    [Non_alpha2_quotes] => +
    [Non_alpha3_quotes] => *
    [Non_alpha4_quotes] => %
    [Non_alpha5_quotes] => <>
    [Non_alpha6_quotes] => @
    [Non_alpha7_quotes] => #
    [Non_alpha8_quotes] => ^
    [Non_alpha9_quotes] => -
    [Non_alpha10_quotes] => =
    [Non_alpha11_quotes] => :
    [Non_alpha12_quotes] => ?
    [Non_alpha13_quotes] => /
    [Non_alpha15_quotes] => &
    [Non_alpha16_quotes] => {}
    [Non_alpha17_quotes] => |
    [Non_alpha18_quotes] => ~
    [Non_alpha19_quotes] => !
    [Non_alpha21_quotes] => ()
    [Non_alpha_string1] => Hello@world
    [Non_alpha_string1_quotes] => Hello@world
    [Non_alpha_string2_quotes] => Hello!world
    [Non_alpha_string3_quotes] => Hello#world
    [Non_alpha_string4_quotes] => Hello&world
    [Non_alpha_string5_quotes] => Hello*world
    [Non_alpha_string6_quotes] => Hello+world
    [Non_alpha_string7_quotes] => Hello-world
    [Non_alpha_string8_quotes] => Hello'world
    [Non_alpha_string9_quotes] => Hello:world
    [Non_alpha_string10_quotes] => Hello;world
    [Non_alpha_string11_quotes] => Hello<world
    [Non_alpha_string12_quotes] => Hello>world
    [Non_alpha_string13_quotes] => Hello>world
    [Non_alpha_string14_quotes] => Hello?world
    [Non_alpha_string15_quotes] => Hello\world
    [Non_alpha_string16_quotes] => Hello^world
    [Non_alpha_string17_quotes] => Hello_world
    [Non_alpha_string18_quotes] => Hello|world
    [Non_alpha_string19_quotes] => Hello~world
    [Non_alpha_string20_quotes] => Hello`world
    [Non_alpha_string21_quotes] => Hello(world)
    [String1] => Hello, world
Good Morning
    [String2] => 
Hello, world
             Good Morning

    [String3] => Hello, world	Good Morning
    [String4] => 

    [String5] => 


    [String6] => Hello, world	Good Morning
    [Key1] => 1
    [Key2] => 1
    [Key3] => 1
    [Key4] => 
    [Key5] => 
    [Key6] => 
    [Key7] => 1
    [Key8] => 1
    [Key9] => 1
    [Key10] => 1
    [Key11] => 
    [Key12] => 
    [Key13] => 
    [Key14] => 
    [Key15] => 
    [Key16] => 
    [Key17] => 
    [Key18] => 
)

-- ini file with process_sections as TRUE --
Array
(
    [Constans] => Array
        (
            [one] => 1
            [five] => 5
            [animal] => Humming bird
            [Language] => PHP
            [PHP_CONSTANT] => 1.2345678
            [10] => Ten
            [HELLO] => HELLO
        )

    [date] => Array
        (
            [date] => 
            [time] => 
        )

    [paths] => Array
        (
            [path] => /usr/local/bin
            [URL] => http://www.php.net
        )

    [Decimal] => Array
        (
            [Decimal_value1] => 100
            [Decimal_value2] => -100
            [Decimal_value3] => -2147483647
            [Decimal_value4] => 2147483647
            [Decimal_value5] => -2147483648
            [Decimal_value6] => 2147483648
        )

    [Octal] => Array
        (
            [Octal_value] => 0100
        )

    [Hex] => Array
        (
            [Hex_value1] => 0x101
            [Hex_Value2] => 0x103
        )

    [Non-alphanumerics_as_values] => Array
        (
            [Non_alpha1] => 
            [Non_alpha2] => +
            [Non_alpha3] => *
            [Non_alpha4] => %
            [Non_alpha5] => <>
            [Non_alpha6] => @
            [Non_alpha7] => #
            [Non_alpha8] => -
            [Non_alpha9] => :
            [Non_alpha10] => ?
            [Non_alpha11] => /
            [Non_alpha12] => \
            [Non_alpha1_quotes] => ;
            [Non_alpha2_quotes] => +
            [Non_alpha3_quotes] => *
            [Non_alpha4_quotes] => %
            [Non_alpha5_quotes] => <>
            [Non_alpha6_quotes] => @
            [Non_alpha7_quotes] => #
            [Non_alpha8_quotes] => ^
            [Non_alpha9_quotes] => -
            [Non_alpha10_quotes] => =
            [Non_alpha11_quotes] => :
            [Non_alpha12_quotes] => ?
            [Non_alpha13_quotes] => /
            [Non_alpha15_quotes] => &
            [Non_alpha16_quotes] => {}
            [Non_alpha17_quotes] => |
            [Non_alpha18_quotes] => ~
            [Non_alpha19_quotes] => !
            [Non_alpha21_quotes] => ()
        )

    [Non-alpha numerics in strings] => Array
        (
            [Non_alpha_string1] => Hello@world
        )

    [Non-alpha numerics in strings -with quotes] => Array
        (
            [Non_alpha_string1_quotes] => Hello@world
            [Non_alpha_string2_quotes] => Hello!world
            [Non_alpha_string3_quotes] => Hello#world
            [Non_alpha_string4_quotes] => Hello&world
            [Non_alpha_string5_quotes] => Hello*world
            [Non_alpha_string6_quotes] => Hello+world
            [Non_alpha_string7_quotes] => Hello-world
            [Non_alpha_string8_quotes] => Hello'world
            [Non_alpha_string9_quotes] => Hello:world
            [Non_alpha_string10_quotes] => Hello;world
            [Non_alpha_string11_quotes] => Hello<world
            [Non_alpha_string12_quotes] => Hello>world
            [Non_alpha_string13_quotes] => Hello>world
            [Non_alpha_string14_quotes] => Hello?world
            [Non_alpha_string15_quotes] => Hello\world
            [Non_alpha_string16_quotes] => Hello^world
            [Non_alpha_string17_quotes] => Hello_world
            [Non_alpha_string18_quotes] => Hello|world
            [Non_alpha_string19_quotes] => Hello~world
            [Non_alpha_string20_quotes] => Hello`world
            [Non_alpha_string21_quotes] => Hello(world)
        )

    [Newlines_in_Values] => Array
        (
            [String1] => Hello, world
Good Morning
            [String2] => 
Hello, world
             Good Morning

            [String3] => Hello, world	Good Morning
            [String4] => 

            [String5] => 


            [String6] => Hello, world	Good Morning
        )

    [ReservedKeys_as_Values] => Array
        (
            [Key1] => 1
            [Key2] => 1
            [Key3] => 1
            [Key4] => 
            [Key5] => 
            [Key6] => 
            [Key7] => 1
            [Key8] => 1
            [Key9] => 1
            [Key10] => 1
            [Key11] => 
            [Key12] => 
            [Key13] => 
            [Key14] => 
            [Key15] => 
            [Key16] => 
            [Key17] => 
            [Key18] => 
        )

    [ReservedKeys_as_Keys] => Array
        (
        )

)
*** Done **

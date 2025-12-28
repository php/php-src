--TEST--
GH-16013 (endianness issue with FFI)
--EXTENSIONS--
ffi
zend_test
--FILE--
<?php
require_once('utils.inc');

$header = <<<HEADER
enum bug_gh16013_enum {
	BUG_GH16013_A = 1,
	BUG_GH16013_B = 2,
};
struct bug_gh16013_int_struct {
	int field;
};
struct bug_gh16013_callback_struct {
	int8_t (*return_int8)(int8_t);
	uint8_t (*return_uint8)(uint8_t);
	int16_t (*return_int16)(int16_t);
	uint16_t (*return_uint16)(uint16_t);
	int32_t (*return_int32)(int32_t);
	uint32_t (*return_uint32)(uint32_t);
	float (*return_float)(float);
	struct bug_gh16013_int_struct (*return_struct)(struct bug_gh16013_int_struct);
	enum bug_gh16013_enum (*return_enum)(enum bug_gh16013_enum);
};

char bug_gh16013_return_char();
bool bug_gh16013_return_bool();
short bug_gh16013_return_short();
int bug_gh16013_return_int();
enum bug_gh16013_enum bug_gh16013_return_enum();
struct bug_gh16013_int_struct bug_gh16013_return_struct();
HEADER;

if (PHP_OS_FAMILY !== 'Windows') {
    $ffi = FFI::cdef($header);
} else {
    try {
        $ffi = FFI::cdef($header, 'php_zend_test.dll');
    } catch (FFI\Exception $ex) {
        $ffi = FFI::cdef($header, ffi_get_php_dll_name());
    }
}

echo "--- Return values ---\n";
var_dump($ffi->bug_gh16013_return_char());
var_dump($ffi->bug_gh16013_return_bool());
var_dump($ffi->bug_gh16013_return_short());
var_dump($ffi->bug_gh16013_return_int());
var_dump($ffi->bug_gh16013_return_enum());
var_dump($ffi->bug_gh16013_return_struct());

echo "--- Callback values ---\n";
$bug_gh16013_callback_struct = $ffi->new('struct bug_gh16013_callback_struct');
$bug_gh16013_callback_struct->return_int8 = function($val) use($ffi) {
    $cdata = $ffi->new('int8_t');
    $cdata->cdata = $val;
    return $cdata;
};
$bug_gh16013_callback_struct->return_uint8 = function($val) use($ffi) {
    $cdata = $ffi->new('uint8_t');
    $cdata->cdata = $val;
    return $cdata;
};
$bug_gh16013_callback_struct->return_int16 = function($val) use($ffi) {
    $cdata = $ffi->new('int16_t');
    $cdata->cdata = $val;
    return $cdata;
};
$bug_gh16013_callback_struct->return_uint16 = function($val) use($ffi) {
    $cdata = $ffi->new('uint16_t');
    $cdata->cdata = $val;
    return $cdata;
};
$bug_gh16013_callback_struct->return_int32 = function($val) use($ffi) {
    $cdata = $ffi->new('int32_t');
    $cdata->cdata = $val;
    return $cdata;
};
$bug_gh16013_callback_struct->return_uint32 = function($val) use($ffi) {
    $cdata = $ffi->new('uint32_t');
    $cdata->cdata = $val;
    return $cdata;
};
$bug_gh16013_callback_struct->return_float = function($val) use($ffi) {
    $cdata = $ffi->new('float');
    $cdata->cdata = $val;
    return $cdata;
};
$bug_gh16013_callback_struct->return_struct = function($val) use($ffi) {
    return $val;
};
$bug_gh16013_callback_struct->return_enum = function($val) use($ffi) {
    $cdata = $ffi->new('enum bug_gh16013_enum');
    $cdata->cdata = $val;
    return $cdata;
};

var_dump(($bug_gh16013_callback_struct->return_int8)(-4));
var_dump(($bug_gh16013_callback_struct->return_uint8)(4));
var_dump(($bug_gh16013_callback_struct->return_int16)(-10000));
var_dump(($bug_gh16013_callback_struct->return_uint16)(10000));
var_dump(($bug_gh16013_callback_struct->return_int32)(-100000));
var_dump(($bug_gh16013_callback_struct->return_uint32)(100000));
var_dump(($bug_gh16013_callback_struct->return_float)(12.34));
$struct = $ffi->new('struct bug_gh16013_int_struct');
$struct->field = 10;
var_dump(($bug_gh16013_callback_struct->return_struct)($struct));
var_dump(($bug_gh16013_callback_struct->return_enum)($ffi->BUG_GH16013_B));
?>
--EXPECT--
--- Return values ---
string(1) "A"
bool(true)
int(12345)
int(123456789)
int(2)
object(FFI\CData:struct bug_gh16013_int_struct)#2 (1) {
  ["field"]=>
  int(123456789)
}
--- Callback values ---
int(-4)
int(4)
int(-10000)
int(10000)
int(-100000)
int(100000)
float(12.34000015258789)
object(FFI\CData:struct bug_gh16013_int_struct)#13 (1) {
  ["field"]=>
  int(10)
}
int(2)

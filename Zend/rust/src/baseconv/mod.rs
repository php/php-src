use std::ptr;

use libc::c_char;
use libc::c_int;
use libc::size_t;

mod impls;

#[repr(C)]
pub struct BaseConvertResult {
    pub output: *mut c_char,
    pub length: size_t,
    pub overflowed_to_float: c_int,
    pub had_invalid_chars: c_int,
}

#[no_mangle]
pub unsafe extern "C" fn php_rust_base_convert(
    input: *const c_char,
    length: size_t,
    from_base: c_int,
    to_base: c_int,
) -> BaseConvertResult {
    if input.is_null() || length == 0 {
        let result = libc::malloc(2) as *mut c_char;
        if !result.is_null() {
            *result = b'0' as c_char;
            *result.offset(1) = 0;
        }
        return BaseConvertResult {
            output: result,
            length: 1,
            overflowed_to_float: 0,
            had_invalid_chars: 0,
        };
    }

    let input_slice = std::slice::from_raw_parts(input as *const u8, length);

    let convert_result = match impls::convert(input_slice, from_base as u32, to_base as u32) {
        Some(res) => res,
        None => {
            return BaseConvertResult {
                output: ptr::null_mut(),
                length: 0,
                overflowed_to_float: 0,
                had_invalid_chars: 0,
            };
        }
    };

    let output_bytes = convert_result.output;
    let alloc_size = output_bytes.len() + 1;
    let result = libc::malloc(alloc_size) as *mut c_char;
    if result.is_null() {
        return BaseConvertResult {
            output: ptr::null_mut(),
            length: 0,
            overflowed_to_float: 0,
            had_invalid_chars: 0,
        };
    }

    ptr::copy_nonoverlapping(output_bytes.as_ptr(), result as *mut u8, output_bytes.len());
    *result.add(output_bytes.len()) = 0;

    BaseConvertResult {
        output: result,
        length: output_bytes.len(),
        overflowed_to_float: if convert_result.overflowed_to_float {
            1
        } else {
            0
        },
        had_invalid_chars: if convert_result.had_invalid_chars {
            1
        } else {
            0
        },
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use libc::c_void;

    #[test]
    fn test_base_convert() {
        unsafe {
            let input = b"101";
            let result = php_rust_base_convert(input.as_ptr() as *const c_char, input.len(), 2, 16);
            assert!(!result.output.is_null());
            let output = std::slice::from_raw_parts(result.output as *const u8, result.length);
            assert_eq!(output, b"5");
            libc::free(result.output as *mut c_void);
        }
    }

    #[test]
    fn test_invalid_base() {
        unsafe {
            let input = b"123";
            let result = php_rust_base_convert(input.as_ptr() as *const c_char, input.len(), 1, 10);
            assert!(result.output.is_null());
        }
    }
}

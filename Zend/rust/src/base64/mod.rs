use std::ptr;

use libc::c_char;
use libc::c_int;
use libc::c_uchar;
use libc::size_t;

mod impls;

const PHP_BASE64_NO_PADDING: c_int = 1;

#[no_mangle]
pub unsafe extern "C" fn php_rust_base64_encode(
    input: *const c_uchar,
    length: size_t,
    output_len: *mut size_t,
    flags: c_int,
) -> *mut c_char {
    if input.is_null() || length == 0 {
        *output_len = 0;
        let result = libc::malloc(1) as *mut c_char;
        if !result.is_null() {
            *result = 0;
        }
        return result;
    }

    let input_slice = std::slice::from_raw_parts(input, length);
    let no_padding = (flags & PHP_BASE64_NO_PADDING) != 0;
    let encoded = impls::encode(input_slice, no_padding);

    let alloc_size = encoded.len() + 1;
    let result = libc::malloc(alloc_size) as *mut c_char;
    if result.is_null() {
        return ptr::null_mut();
    }

    ptr::copy_nonoverlapping(encoded.as_ptr(), result as *mut u8, encoded.len());
    *result.add(encoded.len()) = 0;
    *output_len = encoded.len();

    result
}

#[no_mangle]
pub unsafe extern "C" fn php_rust_base64_decode(
    input: *const c_uchar,
    length: size_t,
    output_len: *mut size_t,
    strict: c_int,
) -> *mut c_char {
    if input.is_null() || length == 0 {
        *output_len = 0;
        let result = libc::malloc(1) as *mut c_char;
        if !result.is_null() {
            *result = 0;
        }
        return result;
    }

    let input_slice = std::slice::from_raw_parts(input, length);
    let strict_mode = strict != 0;
    let decoded = match impls::decode(input_slice, strict_mode) {
        Some(data) => data,
        None => return ptr::null_mut(),
    };

    let alloc_size = decoded.len() + 1;
    let result = libc::malloc(alloc_size) as *mut c_char;
    if result.is_null() {
        return ptr::null_mut();
    }

    ptr::copy_nonoverlapping(decoded.as_ptr(), result as *mut u8, decoded.len());
    *result.add(decoded.len()) = 0;
    *output_len = decoded.len();

    result
}

#[cfg(test)]
mod tests {
    use super::*;
    use libc::c_void;

    #[test]
    fn test_encode_basic() {
        unsafe {
            let input = b"Hello";
            let mut len = 0;
            let result = php_rust_base64_encode(input.as_ptr(), input.len(), &mut len, 0);
            assert!(!result.is_null());
            let output = std::slice::from_raw_parts(result as *const u8, len);
            assert_eq!(output, b"SGVsbG8=");
            libc::free(result as *mut c_void);
        }
    }

    #[test]
    fn test_round_trip() {
        unsafe {
            let input = b"The quick brown fox jumps over the lazy dog";
            let mut enc_len = 0;
            let encoded = php_rust_base64_encode(input.as_ptr(), input.len(), &mut enc_len, 0);
            assert!(!encoded.is_null());

            let mut dec_len = 0;
            let decoded = php_rust_base64_decode(encoded as *const u8, enc_len, &mut dec_len, 1);
            assert!(!decoded.is_null());

            let output = std::slice::from_raw_parts(decoded as *const u8, dec_len);
            assert_eq!(output, input);

            libc::free(encoded as *mut c_void);
            libc::free(decoded as *mut c_void);
        }
    }
}

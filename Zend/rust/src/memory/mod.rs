use std::ptr;

use libc::c_char;
use libc::c_void;
use libc::size_t;

mod impls;

#[no_mangle]
pub unsafe extern "C" fn zend_rust_strndup(s: *const c_char, n: size_t) -> *mut c_char {
    if s.is_null() {
        return ptr::null_mut();
    }

    if n == usize::MAX {
        return ptr::null_mut();
    }

    let len = libc::strnlen(s, n);
    let input_slice = std::slice::from_raw_parts(s as *const u8, len);

    if let Some(result_vec) = impls::strndup(input_slice, n) {
        let result = libc::malloc(result_vec.len() + 1) as *mut c_char;
        if result.is_null() {
            return ptr::null_mut();
        }

        ptr::copy_nonoverlapping(result_vec.as_ptr(), result as *mut u8, result_vec.len());
        *result.add(result_vec.len()) = 0;

        result
    } else {
        ptr::null_mut()
    }
}

#[no_mangle]
pub unsafe extern "C" fn zend_rust_malloc(size: size_t) -> *mut c_void {
    libc::malloc(size)
}

#[no_mangle]
pub unsafe extern "C" fn zend_rust_free(ptr: *mut c_void) {
    libc::free(ptr);
}

#[no_mangle]
pub unsafe extern "C" fn zend_rust_realloc(ptr: *mut c_void, size: size_t) -> *mut c_void {
    libc::realloc(ptr, size)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_strndup_ffi() {
        unsafe {
            let input = b"hello world\0";
            let result = zend_rust_strndup(input.as_ptr() as *const c_char, 5);

            assert!(!result.is_null());

            let mut i = 0;
            while i < 5 {
                assert_eq!(*result.add(i), input[i] as c_char);
                i += 1;
            }
            assert_eq!(*result.add(5), 0);

            libc::free(result as *mut c_void);
        }
    }

    #[test]
    fn test_malloc_free() {
        unsafe {
            let ptr = zend_rust_malloc(100);
            assert!(!ptr.is_null());

            *(ptr as *mut u8) = 42;
            assert_eq!(*(ptr as *mut u8), 42);

            zend_rust_free(ptr);
        }
    }

    #[test]
    fn test_realloc() {
        unsafe {
            let ptr = zend_rust_malloc(10);
            assert!(!ptr.is_null());

            *(ptr as *mut u8) = 42;

            let new_ptr = zend_rust_realloc(ptr, 100);
            assert!(!new_ptr.is_null());

            assert_eq!(*(new_ptr as *mut u8), 42);

            zend_rust_free(new_ptr);
        }
    }
}

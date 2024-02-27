use std::ffi::{CString, c_char};


#[no_mangle]
pub extern "C" fn domxpath_quote_literal(input: *const c_char, len: *mut usize) -> *mut c_char {
    let slice = unsafe { std::slice::from_raw_parts(input as *const u8, *len as usize) };

    let single_quote_absent = !slice.contains(&b'\'');
    let double_quote_absent = !slice.contains(&b'"');

    let result = if single_quote_absent {
        let mut res = Vec::with_capacity(slice.len() + 2);
        res.push(b'\'');
        res.extend_from_slice(slice);
        res.push(b'\'');
        res
    } else if double_quote_absent {
        let mut res = Vec::with_capacity(slice.len() + 2);
        res.push(b'"');
        res.extend_from_slice(slice);
        res.push(b'"');
        res
    } else {
        let mut res = Vec::from("concat(".as_bytes());
        let mut temp_slice = slice;

        while !temp_slice.is_empty() {
            let bytes_until_single_quote = temp_slice.iter().position(|&x| x == b'\'').unwrap_or(temp_slice.len());
            let bytes_until_double_quote = temp_slice.iter().position(|&x| x == b'"').unwrap_or(temp_slice.len());
            
            let (quote_method, bytes_until_quote) = if bytes_until_single_quote > bytes_until_double_quote {
                (b'\'', bytes_until_single_quote)
            } else {
                (b'"', bytes_until_double_quote)
            };

            res.push(quote_method);
            res.extend_from_slice(&temp_slice[..bytes_until_quote]);
            res.push(quote_method);
            res.push(b',');
            temp_slice = &temp_slice[bytes_until_quote..];
        }
        let res_len = res.len();
        res[res_len - 1] = b')';
        res
    };

    // Update length
    unsafe {
        *len = result.len() as usize;
    }

    // Convert Vec<u8> to *mut c_char
    let c_str = CString::new(result).expect("CString::new failed");
    c_str.into_raw()
}
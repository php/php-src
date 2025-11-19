/// Lookup table for mapping ASCII bytes to integer values (0-35).
/// 0xFF indicates an invalid character for base conversion.
const DIGIT_LUT: [u8; 256] = {
    let mut table = [0xFF; 256];
    let mut i = 0u8;

    // 0-9
    while i <= 9 {
        table[(b'0' + i) as usize] = i;
        i += 1;
    }
    // A-Z
    i = 0;
    while i < 26 {
        table[(b'A' + i) as usize] = 10 + i;
        i += 1;
    }
    // a-z
    i = 0;
    while i < 26 {
        table[(b'a' + i) as usize] = 10 + i;
        i += 1;
    }

    table
};

const OUTPUT_DIGITS: &[u8; 36] = b"0123456789abcdefghijklmnopqrstuvwxyz";

pub struct Result {
    pub output: Vec<u8>,
    pub overflowed_to_float: bool,
    pub had_invalid_chars: bool,
}

pub fn convert(input: &[u8], from_base: u32, to_base: u32) -> Option<Result> {
    if from_base < 2 || from_base > 36 || to_base < 2 || to_base > 36 {
        return None;
    }

    if input.is_empty() {
        return Some(Result {
            output: vec![b'0'],
            overflowed_to_float: false,
            had_invalid_chars: false,
        });
    }

    let (trimmed, _) = prepare_input(input, from_base);

    if trimmed.is_empty() {
        return Some(Result {
            output: vec![b'0'],
            overflowed_to_float: false,
            had_invalid_chars: false,
        });
    }

    let (val_int, val_float, is_float, had_invalid) = parse_with_mode_switch(trimmed, from_base);

    let output = if is_float {
        fast_long_to_base(val_float as i64, to_base)
    } else {
        fast_long_to_base(val_int, to_base)
    };

    Some(Result {
        output,
        overflowed_to_float: is_float,
        had_invalid_chars: had_invalid,
    })
}

#[inline(always)]
fn prepare_input(input: &[u8], base: u32) -> (&[u8], bool) {
    let len = input.len();
    let mut start = 0;

    while start < len {
        let b = input[start];
        if !matches!(b, b' ' | b'\t' | b'\n' | b'\r' | 0x0B | 0x0C) {
            break;
        }
        start += 1;
    }

    let mut end = len;
    while end > start {
        let b = input[end - 1];
        if !matches!(b, b' ' | b'\t' | b'\n' | b'\r' | 0x0B | 0x0C) {
            break;
        }
        end -= 1;
    }

    if start >= end {
        return (&[], false);
    }

    let mut slice = &input[start..end];

    if slice.len() >= 2 && slice[0] == b'0' {
        match (slice[1], base) {
            (b'x' | b'X', 16) => slice = &slice[2..],
            (b'o' | b'O', 8) => slice = &slice[2..],
            (b'b' | b'B', 2) => slice = &slice[2..],
            _ => {}
        }
    }

    (slice, false)
}

/// Parses input. Starts as i64, switches to f64 on overflow dynamically.
/// Returns (i64_value, f64_value, is_float_mode, had_invalid)
#[inline(always)]
fn parse_with_mode_switch(data: &[u8], from_base: u32) -> (i64, f64, bool, bool) {
    let mut val_i64: i64 = 0;
    let mut val_f64: f64 = 0.0;
    let mut mode_float = false;
    let mut had_invalid = false;

    let base_i64 = from_base as i64;
    let base_u8 = from_base as u8;

    let safe_mul_limit = i64::MAX / base_i64;

    for &byte in data {
        let digit = DIGIT_LUT[byte as usize];

        if digit >= base_u8 {
            had_invalid = true;
            continue;
        }

        if mode_float {
            val_f64 = val_f64.mul_add(from_base as f64, digit as f64);
        } else {
            if val_i64 > safe_mul_limit {
                mode_float = true;
                val_f64 = (val_i64 as f64).mul_add(from_base as f64, digit as f64);
            } else {
                let next_val = val_i64 * base_i64;
                if next_val > i64::MAX - (digit as i64) {
                    mode_float = true;
                    val_f64 = (next_val as f64) + (digit as f64);
                } else {
                    val_i64 = next_val + (digit as i64);
                }
            }
        }
    }

    (val_i64, val_f64, mode_float, had_invalid)
}

#[inline(always)]
fn fast_long_to_base(mut value: i64, to_base: u32) -> Vec<u8> {
    if value == 0 {
        return vec![b'0'];
    }

    if value < 0 {
        return vec![b'0'];
    }

    let mut buffer = [0u8; 64];
    let mut ptr = 64;

    if to_base.is_power_of_two() {
        let shift = to_base.trailing_zeros();
        let mask = (to_base - 1) as i64;

        while value > 0 {
            ptr -= 1;
            // SAFETY: ptr is between 0 and 63, mask & value < 36
            buffer[ptr] = OUTPUT_DIGITS[(value & mask) as usize];
            value >>= shift;
        }
    } else {
        let base_i64 = to_base as i64;
        while value > 0 {
            ptr -= 1;
            let rem = (value % base_i64) as usize;
            // SAFETY: rem is < base < 36
            buffer[ptr] = OUTPUT_DIGITS[rem];
            value /= base_i64;
        }
    }

    buffer[ptr..64].to_vec()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_convert_basic() {
        let result = convert(b"101", 2, 16).unwrap();
        assert_eq!(result.output, b"5");
        assert!(!result.overflowed_to_float);
        assert!(!result.had_invalid_chars);
    }

    #[test]
    fn test_convert_hex_to_dec() {
        let result = convert(b"FF", 16, 10).unwrap();
        assert_eq!(result.output, b"255");
    }

    #[test]
    fn test_invalid_base() {
        assert!(convert(b"123", 1, 10).is_none());
        assert!(convert(b"123", 10, 37).is_none());
    }

    #[test]
    fn test_overflow_logic() {
        let input = b"9223372036854775808";
        let result = convert(input, 10, 10).unwrap();
        assert!(result.overflowed_to_float);
    }

    #[test]
    fn test_whitespace_and_prefix() {
        let result = convert(b"  0xFF  ", 16, 10).unwrap();
        assert_eq!(result.output, b"255");
    }
}

const TABLE: &[u8; 64] = b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const PAD: u8 = b'=';
const REVERSE: [i8; 256] = [
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2, -2, 0, 1, 2, 3, 4, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2, -2, 26,
    27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
];

pub fn encode(input: &[u8], no_padding: bool) -> Vec<u8> {
    if input.is_empty() {
        return Vec::new();
    }

    let encoded_len = if no_padding {
        (input.len() * 4).div_ceil(3)
    } else {
        input.len().div_ceil(3) * 4
    };

    let mut output = Vec::with_capacity(encoded_len);
    let mut i = 0;

    while i + 2 < input.len() {
        let b0 = input[i] as usize;
        let b1 = input[i + 1] as usize;
        let b2 = input[i + 2] as usize;

        output.push(TABLE[(b0 >> 2) & 0x3F]);
        output.push(TABLE[((b0 << 4) | (b1 >> 4)) & 0x3F]);
        output.push(TABLE[((b1 << 2) | (b2 >> 6)) & 0x3F]);
        output.push(TABLE[b2 & 0x3F]);

        i += 3;
    }

    let remaining = input.len() - i;
    if remaining > 0 {
        let b0 = input[i] as usize;
        output.push(TABLE[(b0 >> 2) & 0x3F]);

        if remaining == 1 {
            output.push(TABLE[(b0 << 4) & 0x3F]);
            if !no_padding {
                output.push(PAD);
                output.push(PAD);
            }
        } else {
            let b1 = input[i + 1] as usize;
            output.push(TABLE[((b0 << 4) | (b1 >> 4)) & 0x3F]);
            output.push(TABLE[(b1 << 2) & 0x3F]);
            if !no_padding {
                output.push(PAD);
            }
        }
    }

    output
}

pub fn decode(input: &[u8], strict: bool) -> Option<Vec<u8>> {
    if input.is_empty() {
        return Some(Vec::new());
    }

    let mut output = Vec::with_capacity((input.len() * 3) / 4);
    let mut state = 0;
    let mut acc: u32 = 0;
    let mut padding_seen = false;
    let mut padding_count = 0;

    for &ch in input {
        if ch == PAD {
            padding_seen = true;
            padding_count += 1;

            if strict {
                if state < 2 {
                    return None;
                }
                continue;
            } else {
                continue;
            }
        }

        let value = REVERSE[ch as usize];

        if value == -1 {
            continue;
        }

        if value == -2 {
            if strict {
                return None;
            }
            continue;
        }

        if strict && padding_seen {
            return None;
        }

        acc = (acc << 6) | (value as u32);
        state += 1;

        if state == 4 {
            output.push(((acc >> 16) & 0xFF) as u8);
            output.push(((acc >> 8) & 0xFF) as u8);
            output.push((acc & 0xFF) as u8);
            state = 0;
            acc = 0;
        }
    }

    if state > 0 {
        if strict && state == 1 {
            return None;
        }

        if strict && padding_seen {
            let expected_padding = match state {
                2 => 2,
                3 => 1,
                _ => 0,
            };

            if padding_count != expected_padding {
                return None;
            }
        }

        if state == 2 {
            output.push(((acc >> 4) & 0xFF) as u8);
        } else if state == 3 {
            output.push(((acc >> 10) & 0xFF) as u8);
            output.push(((acc >> 2) & 0xFF) as u8);
        }
    } else if strict && padding_count > 0 {
        return None;
    }

    Some(output)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_encode_basic() {
        assert_eq!(encode(b"Hello", false), b"SGVsbG8=");
    }

    #[test]
    fn test_encode_no_padding() {
        assert_eq!(encode(b"Hello", true), b"SGVsbG8");
    }

    #[test]
    fn test_decode_basic() {
        assert_eq!(decode(b"SGVsbG8=", false), Some(b"Hello".to_vec()));
    }

    #[test]
    fn test_decode_strict_invalid() {
        assert_eq!(decode(b"SGVs!!!bG8=", true), None);
    }

    #[test]
    fn test_decode_strict_wrong_padding() {
        assert_eq!(decode(b"VV=", true), None);
    }

    #[test]
    fn test_decode_strict_correct_padding() {
        assert_eq!(decode(b"VV==", true), Some(b"U".to_vec()));
    }

    #[test]
    fn test_round_trip() {
        let input = b"The quick brown fox jumps over the lazy dog";
        let encoded = encode(input, false);
        let decoded = decode(&encoded, true).unwrap();
        assert_eq!(decoded, input);
    }
}

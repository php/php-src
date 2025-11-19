pub fn strndup(s: &[u8], n: usize) -> Option<Vec<u8>> {
    let len = n.min(s.len());
    Some(s[..len].to_vec())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_strndup_basic() {
        let result = strndup(b"hello world", 5).unwrap();
        assert_eq!(result, b"hello");
    }

    #[test]
    fn test_strndup_empty() {
        let result = strndup(b"", 0).unwrap();
        assert_eq!(result, b"");
    }

    #[test]
    fn test_strndup_longer_than_input() {
        let result = strndup(b"test", 100).unwrap();
        assert_eq!(result, b"test");
    }
}
